#include <stdio.h>
#include <ndds_cpp.h>
#include "ice.h"
#include "iceSupport.h"
#include "icePlugin.h"

void clamp() {
  DDSDomainParticipantFactory *factory = DDSTheParticipantFactory->get_instance();
  DDSDomainParticipant *participant = factory->create_participant(15, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
  ice::InfusionObjectiveTypeSupport::register_type(participant, ice::InfusionObjectiveTYPENAME);
  ice::ClampStatusTypeSupport::register_type(participant, ice::ClampStatusTYPENAME);
  
  DDSTopic *infusionObjectiveTopic = participant->create_topic(ice::InfusionObjectiveTopic, ice::InfusionObjectiveTYPENAME, DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
  DDSTopic *clampStatusTopic = participant->create_topic(ice::ClampStatusTopic, ice::ClampStatusTYPENAME, DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);

  DDS_StringSeq params;
  params.ensure_length(1,1);
  params[0] = "'MYUDI'";
  DDSContentFilteredTopic *filteredInfusionObjectiveTopic = participant->create_contentfilteredtopic("MyFilteredTopic", infusionObjectiveTopic, "unique_device_identifier = %0", params);

  ice::ClampStatusDataWriter *writer = dynamic_cast<ice::ClampStatusDataWriter*>(participant->create_datawriter(clampStatusTopic, DDS_DATAWRITER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE));
  ice::InfusionObjectiveDataReader *reader = dynamic_cast<ice::InfusionObjectiveDataReader*>(participant->create_datareader(filteredInfusionObjectiveTopic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE));


  ice::ClampStatus clampStatus;
  clampStatus.unique_device_identifier = "MYUDI";
  clampStatus.ready = true;
  clampStatus.door  = true;
  clampStatus.tube  = true;
  clampStatus.clamp = true;
  clampStatus.batteryStatus = 0.84f;
  clampStatus.loadCell = 0.1f;

  DDS_InstanceHandle_t handle = writer->register_instance(clampStatus);

  ice::InfusionObjectiveSeq infusionObjectiveSeq;
  DDS_SampleInfoSeq sampleInfoSeq;


  for(;;) {
  	writer->write(clampStatus, handle);

 	if(DDS_RETCODE_OK == reader->read(infusionObjectiveSeq, sampleInfoSeq, DDS_LENGTH_UNLIMITED, DDS_NOT_READ_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ALIVE_INSTANCE_STATE)) {
 		for(int i = 0; i < infusionObjectiveSeq.length(); ++i) {
 			ice::InfusionObjectiveTypeSupport::print_data(&infusionObjectiveSeq[i]);
 		}
 		reader->return_loan(infusionObjectiveSeq, sampleInfoSeq);
 	}


  	sleep(1);
  }  


  participant->delete_contentfilteredtopic(filteredInfusionObjectiveTopic);

  participant->delete_topic(infusionObjectiveTopic);
  participant->delete_topic(clampStatusTopic);


  ice::InfusionObjectiveTypeSupport::unregister_type(participant);
  ice::ClampStatusTypeSupport::unregister_type(participant);

  factory->delete_participant(participant);

  DDSTheParticipantFactory->finalize_instance();

};

void controller() {
  DDSDomainParticipantFactory *factory = DDSTheParticipantFactory->get_instance();
  DDSDomainParticipant *participant = factory->create_participant(15, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
  ice::InfusionObjectiveTypeSupport::register_type(participant, ice::InfusionObjectiveTYPENAME);
  ice::ClampStatusTypeSupport::register_type(participant, ice::ClampStatusTYPENAME);
  
  DDSTopic *infusionObjectiveTopic = participant->create_topic(ice::InfusionObjectiveTopic, ice::InfusionObjectiveTYPENAME, DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
  DDSTopic *clampStatusTopic = participant->create_topic(ice::ClampStatusTopic, ice::ClampStatusTYPENAME, DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);

  DDS_StringSeq params;
  params.ensure_length(1,1);
  params[0] = "'MYUDI'";
  DDSContentFilteredTopic *filteredClampStatusTopic = participant->create_contentfilteredtopic("MyFilteredTopic", clampStatusTopic, "unique_device_identifier = %0", params);

  ice::ClampStatusDataReader *reader = dynamic_cast<ice::ClampStatusDataReader*>(participant->create_datareader(filteredClampStatusTopic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE));
  ice::InfusionObjectiveDataWriter *writer = dynamic_cast<ice::InfusionObjectiveDataWriter*>(participant->create_datawriter(infusionObjectiveTopic, DDS_DATAWRITER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE));


  ice::InfusionObjective infusionObjective;
  infusionObjective.unique_device_identifier = "MYUDI";
  infusionObjective.requestor = "Controller";
  infusionObjective.stopInfusion = true;

  DDS_InstanceHandle_t handle = writer->register_instance(infusionObjective);

  ice::ClampStatusSeq clampStatusSeq;
  DDS_SampleInfoSeq sampleInfoSeq;


  for(;;) {
  	writer->write(infusionObjective, handle);

 	if(DDS_RETCODE_OK == reader->read(clampStatusSeq, sampleInfoSeq, DDS_LENGTH_UNLIMITED, DDS_NOT_READ_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ALIVE_INSTANCE_STATE)) {
 		for(int i = 0; i < clampStatusSeq.length(); ++i) {
 			ice::ClampStatusTypeSupport::print_data(&clampStatusSeq[i]);
 		}
 		reader->return_loan(clampStatusSeq, sampleInfoSeq);
 	}


  	sleep(1);
  }  


  participant->delete_contentfilteredtopic(filteredClampStatusTopic);

  participant->delete_topic(infusionObjectiveTopic);
  participant->delete_topic(clampStatusTopic);


  ice::InfusionObjectiveTypeSupport::unregister_type(participant);
  ice::ClampStatusTypeSupport::unregister_type(participant);

  factory->delete_participant(participant);

  DDSTheParticipantFactory->finalize_instance();

};


int main(int argc, char ** argv) {
	if(argc < 1) {
		fprintf(stderr, "Specify clamp or controller\n");
		return -1;
	} else {
		if(!strcmp("clamp",argv[1])) {
			fprintf(stderr, "Running as the clamp\n");
			clamp();
		} else if(!strcmp("controller", argv[1])) {
			fprintf(stderr, "Running as the controller\n");
			controller();
		} else {
			fprintf(stderr, "Unknown argument: %s\n", argv[1]);
		}
	}
}