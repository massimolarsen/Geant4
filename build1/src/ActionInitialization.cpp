#include "ActionInitialization.h"
#include "PrimaryGeneratorAction.h"
#include "RunAction.h"
#include "EventAction.h"
#include "SteppingAction.h"


ActionInitialization::ActionInitialization()
	:G4UserActionInitialization()
{}


ActionInitialization::~ActionInitialization()
{}


//Build RunAction globally
void ActionInitialization::BuildForMaster() const
{
	RunAction* runAction = new RunAction;
	SetUserAction(runAction);
}

//Build other classes locally
void ActionInitialization::Build() const
{
	SetUserAction(new PrimaryGeneratorAction);

	RunAction* runAction = newRunAction;
	SetUserAction(runAction);

	EventAction* eventAction = new EventAction(runAction);
	SetUserACtion(eventAction);

	SetUserAction(new SteppingAction(eventAction));
}