#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
	:G4UserSteppingAction(),
	fEventAction(eventAction),
	fScoringVolume(0)

{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	if (!fScoringVolume) {
		const DetectorConstruction* detectorConstruction
			= static_cast<const DetectorConstruction*>
			(G4RunManager::GetRunManager()->GetUserDetectorConstruction());

			fScoringVolume = detectorConstruction->GetScoringVolume();
	}

	//Get volume
	G4LogicalVolume* volume
		= step->GetPreStepPoint()->GetTouchableHandle()
			->GetVolume()->GetLogicalVolume();

	//Check if in volume
	if (volume != fScoringVolume) return;

	//Get energy deposited in step
	G4double edepStep = step->GetTotalEnergyDeposit();
	fEventAction->addEdep(edepStep);
}