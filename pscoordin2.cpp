/*******************************************************************
*
*  DESCRIPTION: class ParallelSCoordinator2
*
*  AUTHOR:	Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE:  24/01/2001
*
*******************************************************************/

/** include files **/
#include "pscoordin2.h"       // header
#include "coupled.h"       // class Coupled
#include "pprocadm.h"	
#include "pmessage.h"
#include "parsimu.h"		//class ParallelMainSimulator

/*******************************************************************
* Function Name: Constructor
********************************************************************/
ParallelSCoordinator2::ParallelSCoordinator2( Coupled * coupled ) 
	: ParallelCoordinator(coupled)
{
	//The ParallelCoordinator constructor has already done much of the work
	parentId = coupled->masterId();
}

/*******************************************************************
* Function Name: initialize
********************************************************************/
void ParallelSCoordinator2::initialize() 
{
	ParallelCoordinator::initialize();

	//Create the list of sibling slaves.

	Coupled& coupled = static_cast<Coupled&> (model());

	for(ModelPartition::const_iterator mcursor = coupled.modelPartition().begin();
	mcursor != coupled.modelPartition().end(); mcursor++) {
		if ( id() != mcursor->second && coupled.masterId() != mcursor->second )
			//Add processor id to the slave list.
			slaves.insert( mcursor->second );
	}	

	//Initialize slave sync count.
	slaveSyncCount = 0;
	collectPhase = false;

	ParallelMainSimulator::Instance().debugStream() << "OK" << endl << flush;
}


/*******************************************************************
* Function Name: receive
* Description: sort the external messages and send the @ message to the inminent childs
********************************************************************/
ParallelProcessor &ParallelSCoordinator2::receive( const CollectMessage &msg ) {

	//cout << "In function ParallelCoordinator::receive(CollectMessage)"<<endl<<flush;
	//Check if we have imminent components

	collectPhase = true;

	nextChange( absoluteNext() - msg.time() );
	lastChange( msg.time() );

	slaveSyncCount += slaves.size();

	//1. Send collect message to inminent childs

	if ( nextChange() == VTime::Zero )
	{
		ParallelCoordinatorState::DependantList::const_iterator cursor;
		CollectMessage collect ( msg.time(), id() );

		for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ ) 
		{
			if ( cursor->second == msg.time() )
			{
				synchronizeList.insert( cursor->first );
				doneCount( doneCount() + 1);
				send( collect , cursor->first );

			}//if
		}		

	} else {

		//We are in the case were there are no imminent components. So doneCount  = 0
		//send all the slave sync.

		sendSlaveSync();

		//If we have already received all slave syncs (it might be possible)
		if( slaveSyncCount == 0) 
		{
			DoneMessage doneMsg( msg.time(), id(), nextChange(), true );
			send( doneMsg, parentId );	
		}	
	} 


	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: sends the * message to the inminent child
********************************************************************/
ParallelProcessor &ParallelSCoordinator2::receive( const InternalMessage &msg )
{

	MASSERTMSG( doneCount() == 0, "Received an InternalMessage and doneCount is not cero!" );	
	MASSERT(slaveSyncCount == 0);

	collectPhase = false;

	//1. Sort the external message queue
	for( MessageBag::iterator extMsgs = externalMsgs.begin(); extMsgs != externalMsgs.end(); extMsgs++ )
	{
		sortExternalMessage( *((BasicExternalMessage*) (*extMsgs)));	
	}

	//2. Empty queue
	externalMsgs.eraseAll();	

	//3. Send all the internal messages
	ProcSet::const_iterator cursor;
	InternalMessage internal( msg.time(), id() ) ;

	for( cursor = synchronizeList.begin(); cursor != synchronizeList.end() ; cursor++ )
	{
		doneCount( doneCount() + 1);
		send( internal, *cursor ) ;
	}

	//Clear the synchronize set
	synchronizeList.erase( synchronizeList.begin(), synchronizeList.end());

	//If there were no imminent components, send a done message
	if ( doneCount() == 0 )
	{
		DoneMessage doneMsg( msg.time(), id(), nextChange(), true );
		send( doneMsg, parentId );	
	} 


	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: receive an InitMessage
********************************************************************/
ParallelProcessor &ParallelSCoordinator2::receive( const InitMessage &msg ) {

	//cout << "In function ParallelCoordinator::receive(InitMessage)"<<endl<<flush;

	//Send an init message to each of the dependants.

	doneCount ( dependants().size() );

	ParallelCoordinatorState::DependantList::const_iterator cursor;
	InitMessage init ( msg.time(), id() );

	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ )
		this->send( init , cursor->first );

	return *this;
}


/*******************************************************************
* Function Name: receive
* Description: sends an X message to the port's influences
********************************************************************/
ParallelCoordinator &ParallelSCoordinator2::sortExternalMessage( const BasicExternalMessage &msg )
{
	//cout << "In function ParallelSCoordinator2::receive(ExternalMessage)"<<endl;

	const InfluenceList &influList( msg.port().influences() ) ;
	InfluenceList::const_iterator cursor( influList.begin() ) ;

	BasicExternalMessage xMsg( msg ) ;
	xMsg.procId( id() ) ;

	for( ; cursor != influList.end(); cursor++ ) {

		//If the destination model has a local master send message
		//to the local master, to the appropiate port
		//Otherwiser, ignore the message.
		if ((*cursor)->model().isLocalMaster()) {

			synchronizeList.insert( (*cursor)->model().localProc());
			xMsg.port( *(*cursor) ) ;
			send( xMsg,(*cursor)->model().localProc()  ) ;

		}//if
	}//for

	return *this ;

}	

/*******************************************************************
* Function Name: receive
* Description: recalculates the inminent child and sends it to his 
*              father inside a done message
********************************************************************/
ParallelProcessor &ParallelSCoordinator2::receive( const DoneMessage &msg )
{
	//cout << "In function ParallelCoordinator::receive(DoneMessage)"<<endl << flush;	

	MASSERTMSG( doneCount() > 0, "Unexpected Done message!" );

	doneCount( doneCount() - 1);

	//Update the depdendant's absolute next time!
	dependants()[msg.procId()] = msg.time() + msg.nextChange();


	if( doneCount() == 0 )
	{

		lastChange ( msg.time() );
		nextChange( calculateNextChange( msg.time() ) );

		//If we are in the collect phase
		if (collectPhase)
		{
			sendSlaveSync();
		}

		//If we have received all the slave sync, then send the done.
		if ( slaveSyncCount == 0)
		{
			DoneMessage doneMsg( msg.time(), id(), nextChange(), true );
			send( doneMsg, parentId );
		}	
	}
	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: translates the output event to a X messages for the 
*              influenced children and to a Y message for his father
********************************************************************/
ParallelCoordinator &ParallelSCoordinator2::sortOutputMessage( const BasicOutputMessage &msg )
{

	Coupled &coupled( static_cast< Coupled & >( model() ) );

	ProcSet procs;		//Remote procs that have already received an external msg.
	bool sentToMaster = false;	//Indicates if the master has received the msg as an output message.

	const InfluenceList &influList( msg.port().influences() );
	InfluenceList::const_iterator cursor( influList.begin() );

	BasicOutputMessage outMsg( msg );
	outMsg.procId( id() );

	BasicExternalMessage extMsg;
	extMsg.time( msg.time() );
	extMsg.procId( id() );
	extMsg.value( msg.value()->clone() );
	extMsg.senderModelId( msg.port().modelId() );

	for( ; cursor != influList.end(); cursor++ )
	{
		// Search for the port in the ouput ports list.
		// If found, send message to parent model.
		// If not found in the output ports list, then it must be an external message
		// If so, check if the recipient has a local master processor.
		// If the recipient does not have a local master processor, find the
		// corresponding slave processor and send the message.

		if( coupled.outputPorts().find( (*cursor)->id() ) == coupled.outputPorts().end() )
		{
			if ( (*cursor)->model().isLocalMaster()) {

				synchronizeList.insert((*cursor)->model().localProc());
				extMsg.port( * (*cursor) );
				send( extMsg, (*cursor)->model().localProc());

			} else {
				//When forwarding a message, do not translate the port

				const ProcId& coordId = (*cursor)->model().parentId();

				//1. The coordinator should not have already been sent the message

				if ( procs.find( coordId ) == procs.end() ) {

					//synchronizeList.insert( coordId );
					procs.insert(coordId);
					extMsg.port( msg.port());		//Use the original port.
					send( extMsg, coordId );

				} // if
			}
		} else
		{
			if( !sentToMaster )
			{
				//When forwarding to master,  do not translate port
				send( outMsg, parentId );
				sentToMaster = true;
			}

		}
	}//for

	return *this;

}

/*******************************************************************
* Function Name: receive
* Description: receive a Slave Sync Msg
********************************************************************/
ParallelProcessor &ParallelSCoordinator2::receive( const OutputSyncMessage & out )
{
	slaveSyncCount--;

	if (slaveSyncCount == 0 && doneCount() == 0)
	{
		DoneMessage doneMsg( lastChange(), id(), nextChange(), true );
		send( doneMsg, parentId );
	}	

	return *this;
}

/*******************************************************************
* Function Name: sendSlaveSync
* Description: sends all the slave sync msgs
********************************************************************/
void ParallelSCoordinator2::sendSlaveSync()
{
	OutputSyncMessage synMsg( lastChange(), id());

	for (ProcSet::const_iterator cursor = slaves.begin(); cursor != slaves.end(); cursor++)
	{
		send( synMsg, *cursor);
	}
}

/*******************************************************************
* Function Name: calculateNextChange
* Description: calculate the time for the next change
********************************************************************/
VTime ParallelSCoordinator2::calculateNextChange(const VTime& time) const
{
	VTime next(VTime::Inf);

	ParallelCoordinatorState::DependantList::const_iterator cursor;

	//Check dependants
	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ )
		if( cursor->second < next) next = cursor->second;

	next -= time;

	return next;

}
