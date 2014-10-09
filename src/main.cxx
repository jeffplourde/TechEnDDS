#include <stdio.h>

/**
 * Includes the DDS C++ API
 * http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/
 */
#include <ndds_cpp.h>

/**
 * Generated data types specific to this example
 */
#include "ice.h"
#include "iceSupport.h"
#include "icePlugin.h"

/***********   RTI DDS INFRASTRUCTURE COMMUNITY **************
To access DDS libraries join our infrastructure community ICE_IC01 at
http://www.rti.com/downloads/rti-dds.html
*/

/**
 * Example function playing the role of the clamp.  Subscribing to the InfusionObjective topic
 * to receive commands and publishing clamp status to the ClampStatus topic.
 */ 
void clamp() {
  /**
   * Access the DomainParticipantFactory singleton to construct DDS entities.
   * 
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/classDDSDomainParticipantFactory.html
   */
  DDSDomainParticipantFactory *factory = DDSTheParticipantFactory->get_instance();

  /**
   * Create a DomainParticipant.  This heavyweight object spools up worker thread pools and enables discovery of 
   * other participants on the network per configuration policies.  
   *
   * The "domain" controls what ports are used in the underlying transport layer.  We tend to use 15 in the lab.
   *
   * Here default Quality of Service settings are used and no listener is registered to receive entity status events.
   *
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/classDDSDomainParticipant.html
   */
  DDSDomainParticipant *participant = factory->create_participant(15, DDS_PARTICIPANT_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);

  /**
   * The user defined types we have created are registered for use by the participant with their default names.
   * Registering the type allows it to be used by readers and writers contained within this DomainParticipant.
   *
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooTypeSupport.html#a25b70f0c36cbb4ba95e6ef08e094e84c
   */
  ice::InfusionObjectiveTypeSupport::register_type(participant, ice::InfusionObjectiveTYPENAME);
  ice::ClampStatusTypeSupport::register_type(participant, ice::ClampStatusTYPENAME);
  
  /**
   * A topic is a string used to match writers and readers in the system.  In addition to its string name each topic
   * is also associated with a data type.  The consistency of the data types on the reading and writing end is 
   * verified at runtime.
   * 
   * Here topic names were included in the IDL definitions as string constants and are therefore available in the generated
   * code as ice::InfusionObjectiveTopic and ice::ClampStatusTopic.
   *
   * Types referenced here were previously registered with the participant by name (ice::InfusionObjectiveTYPENAME and 
   * ice::ClampStatusTYPENAME
   *
   * Here default Quality of Service settings are used and no listener is registered to receive entity status events.
   * 
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/classDDSDomainParticipant.html#a7f249829f7ee94d1461d371800398a21
   */

  DDSTopic *infusionObjectiveTopic = participant->create_topic(ice::InfusionObjectiveTopic, ice::InfusionObjectiveTYPENAME, DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);
  DDSTopic *clampStatusTopic = participant->create_topic(ice::ClampStatusTopic, ice::ClampStatusTYPENAME, DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE);

  /**
   * A ContentFilteredTopic is a special version of a topic that communicates to the writing end a specific filtered interest.
   * In this case we are expressing interest in data instances on the InfusionObjective topic where the unique_device_identifier
   * field is set to 'MYUDI'.
   *
   * This is only one of many ways to filter the contents of a reader.
   *
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/classDDSDomainParticipant.html#a5fd57875fde174c8edc953c32fbd2073
   */
  DDS_StringSeq params;
  params.ensure_length(1,1);
  params[0] = "'MYUDI'";
  DDSContentFilteredTopic *filteredInfusionObjectiveTopic = participant->create_contentfilteredtopic("MyFilteredTopic", infusionObjectiveTopic, "unique_device_identifier = %0", params);

  /**
   * Creates a data writer.  Samples written into this writer will be made available to matched readers per the terms of QoS policies.
   * 
   * The previously created topic is specified for this writer along with default QoS policies and no listener is registered to receive
   * entity status changes.
   * 
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/classDDSDomainParticipant.html#a22d1dd39f39cd30cc45d8f1aede8c515
   */
  ice::ClampStatusDataWriter *writer = dynamic_cast<ice::ClampStatusDataWriter*>(participant->create_datawriter(clampStatusTopic, DDS_DATAWRITER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE));

  /**
   * Create a data reader.  Samples are added to this reader as they are received from remote writers per the terms of QoS policies.
   *
   * The previously created topic is specified for this reader along with default QoS policies and no listener is registered to receive
   * entity status changes.
   *
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/classDDSDomainParticipant.html#ac0e5f00a45b4f39a96441a19d9d88a0d
   */
  ice::InfusionObjectiveDataReader *reader = dynamic_cast<ice::InfusionObjectiveDataReader*>(participant->create_datareader(filteredInfusionObjectiveTopic, DDS_DATAREADER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE));


  /**
   * Builds and populates a ClampStatus object to be written
   */
  ice::ClampStatus clampStatus;
  clampStatus.unique_device_identifier = "MYUDI";
  clampStatus.ready = true;
  clampStatus.door  = true;
  clampStatus.tube  = true;
  clampStatus.clamp = true;
  clampStatus.batteryStatus = 0.84f;
  clampStatus.loadCell = 0.1f;

  /**
   * Registering the instance allows the writer to precalculate the instance key
   * (which is a hash of all the instance's key fields; in this case unique_device_identifier)
   *
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooDataWriter.html#a5a9c7ff3c6f5c559c56ee3481031ae3a
   */
  DDS_InstanceHandle_t handle = writer->register_instance(clampStatus);

  /** 
   * These sequence structures will receive samples read out of the data reader.  The first will
   * contain the data samples, the second contains metadata about those samples (known as DDS_SampleInfo)
   * 
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooSeq.html
   * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structDDS__SampleInfoSeq.html
   */
  ice::InfusionObjectiveSeq infusionObjectiveSeq;
  DDS_SampleInfoSeq sampleInfoSeq;


  /**
   * Right now this example will perpetually write to ClampStatus and read from InfusionObjective.
   * Probably a check of stdin or a SIGINT handler should break out of it gracefully.
   */
  for(;;) {
  	/**
  	 * Writes the new ClampStatus using the pre-registered handle
  	 *
  	 * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooDataWriter.html#abb3770f202340bc819368463987eb055
  	 */
  	writer->write(clampStatus, handle);

  	/**
  	 * Examines the reader for any new (NOT_READ) samples from ALIVE instances.  If the return code
  	 * is DDS_RETCODE_OK then samples were placed into the specified sequence objects.
  	 *
  	 * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooDataReader.html
  	 */
 	if(DDS_RETCODE_OK == reader->read(infusionObjectiveSeq, sampleInfoSeq, DDS_LENGTH_UNLIMITED, DDS_NOT_READ_SAMPLE_STATE, DDS_ANY_VIEW_STATE, DDS_ALIVE_INSTANCE_STATE)) {
		/**
		 * Iterate through the contents of the sequence
		 */ 		
 		for(int i = 0; i < infusionObjectiveSeq.length(); ++i) {
 			/**
 			 * print_data is a generated convenience method for dumping sample contents to stdout
 			 *
 			 * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooTypeSupport.html#a1f2301d640990cccdcabd38a3dbd80de
 			 */
 			ice::InfusionObjectiveTypeSupport::print_data(&infusionObjectiveSeq[i]);
 		}
 		/**
 		 * Returns borrowed sample instances to the reader.
 		 *
 		 * See http://community.rti.com/rti-doc/510/ndds.5.1.0/doc/html/api_cpp/structFooDataReader.html#a9f142a4ee87d08e14a4c07325e1b13ea
 		 */
 		reader->return_loan(infusionObjectiveSeq, sampleInfoSeq);
 	}


  	sleep(1);
  }  

  /**
   * The teardown logic here closely mirrors the earlier creation sequence.
   */
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


/**
 * The program can run in clamp or controller mode; as specified by a single
 * command line argument
 */
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
	return 0;
}