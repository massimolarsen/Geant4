//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B2aDetectorConstruction.cc
/// \brief Implementation of the B2aDetectorConstruction class

#include "B2aDetectorConstruction.hh"
#include "B2aDetectorMessenger.hh"
#include "B2TrackerSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4RotationMatrix.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* B2aDetectorConstruction::fMagFieldMessenger = 0;

B2aDetectorConstruction::B2aDetectorConstruction()
	:G4VUserDetectorConstruction(),
	fNbOfChambers(0),
	fLogicTarget(NULL), fLogicChamber(NULL),
	fTargetMaterial(NULL), fChamberMaterial(NULL),
	fStepLimit(NULL),
	fCheckOverlaps(true)
{
	fMessenger = new B2aDetectorMessenger(this);

	fNbOfChambers = 16;
	fLogicChamber = new G4LogicalVolume * [fNbOfChambers];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2aDetectorConstruction::~B2aDetectorConstruction()
{
	delete[] fLogicChamber;
	delete fStepLimit;
	delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2aDetectorConstruction::Construct()
{
	// Define materials
	DefineMaterials();

	// Define volumes
	return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::DefineMaterials()
{
	// Material definition 

	G4NistManager* nistManager = G4NistManager::Instance();

	// Air defined using NIST Manager
	nistManager->FindOrBuildMaterial("G4_AIR");

	//LSO

	G4bool isotopes = false;

	G4Element* O = nistManager->FindOrBuildElement("O", isotopes);
	G4Element* Si = nistManager->FindOrBuildElement("Si", isotopes);
	G4Element* Lu = nistManager->FindOrBuildElement("Lu", isotopes);

	G4Material* LSO = new G4Material("Lu2SiO5", 7.4 * g / cm3, 3);
	LSO->AddElement(Lu, 2);
	LSO->AddElement(Si, 1);
	LSO->AddElement(O, 5);

	fChamberMaterial = nistManager->FindOrBuildMaterial("Lu2SiO5");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2aDetectorConstruction::DefineVolumes()
{
	G4Material* air = G4Material::GetMaterial("G4_AIR");

	// Sizes of the principal geometrical components (solids)

	G4double chamberSpacing = 10 * cm; // from chamber center to center!

	G4double chamberWidth = 10.0 * cm; // width of the chambers

	G4double trackerLength = (fNbOfChambers + 1) * chamberSpacing;

	G4double worldLength = 40 * cm;

	G4double trackerSize = 0.5 * trackerLength;  // Half length of the Tracker

	// Definitions of Solids, Logical Volumes, Physical Volumes

	// World

	G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

	G4cout << "Computed tolerance = "
		<< G4GeometryTolerance::GetInstance()->GetSurfaceTolerance() / mm
		<< " mm" << G4endl;

	G4Box* worldS
		= new G4Box("world",                                    //its name
			worldLength / 2, worldLength / 2, worldLength / 2); //its size
	G4LogicalVolume* worldLV
		= new G4LogicalVolume(
			worldS,   //its solid
			air,      //its material
			"World"); //its name

//  Must place the World Physical volume unrotated at (0,0,0).
// 
	G4VPhysicalVolume* worldPV
		= new G4PVPlacement(
			0,               // no rotation
			G4ThreeVector(), // at (0,0,0)
			worldLV,         // its logical volume
			"World",         // its name
			0,               // its mother  volume
			false,           // no boolean operations
			0,               // copy number
			fCheckOverlaps); // checking overlaps 

// Tracker

	G4ThreeVector positionTracker = G4ThreeVector(0, 0, 0);

	G4Tubs* trackerS
		= new G4Tubs("tracker", 0, trackerSize, trackerSize, 0. * deg, 360. * deg);
	G4LogicalVolume* trackerLV
		= new G4LogicalVolume(trackerS,
			air,
			"Tracker",
			0, 0, 0);
	new G4PVPlacement(0,               // no rotation
		positionTracker, // at (x,y,z)
		trackerLV,       // its logical volume
		"Tracker",       // its name
		worldLV,         // its mother  volume
		false,           // no boolean operations
		0,               // copy number
		fCheckOverlaps); // checking overlaps 

// Visualization attributes

	G4VisAttributes* boxVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
	G4VisAttributes* chamberVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0));

	worldLV->SetVisAttributes(boxVisAtt);
	//fLogicTarget ->SetVisAttributes(boxVisAtt);
	//trackerLV    ->SetVisAttributes(boxVisAtt);

	// Tracker segments

	G4cout << "There are " << fNbOfChambers << " chambers in the tracker region. "
		<< G4endl
		<< "The chambers are " << chamberWidth / cm << " cm of "
		<< fChamberMaterial->GetName() << G4endl
		<< "The distance between chamber is " << chamberSpacing / cm << " cm"
		<< G4endl;

	//G4RotationMatrix* myRotation = new G4RotationMatrix();
	//	myRotation->rotateX(90. * deg);
	//	myRotation->rotateY(0. * deg);
	//	myRotation->rotateZ(0. * deg);

	for (G4int i = 0; i < 4; i++) {
		G4double Yposition = i * 4.8;

		for (G4int j = 0; j < 4; j++) {
			G4double Xposition = j * 4.8;
			G4int copyNo = i * 4 + j;


			//G4Tubs* chamberS
			//	= new G4Tubs("Chamber_solid", 0, 1.5 * cm, .5 * cm, 180. * deg, 360. * deg);

			G4Box* chamberS
				= new G4Box("Chamber_solid", 2.4 * cm, 2.4 * cm, 1 * cm);

			fLogicChamber[copyNo] =
				new G4LogicalVolume(chamberS, fChamberMaterial, "Chamber_LV", 0, 0, 0);

			fLogicChamber[copyNo]->SetVisAttributes(chamberVisAtt);

			new G4PVPlacement(0,              // no rotation
				G4ThreeVector(Xposition * cm, Yposition * cm, 0), // at (x,y,z)
				fLogicChamber[copyNo],        // its logical volume
				"Chamber_PV",                 // its name
				trackerLV,                    // its mother  volume
				false,                        // no boolean operations
				copyNo,                       // copy number
				fCheckOverlaps);              // checking overlaps 

		}
	}

	// Example of User Limits
	//
	// Below is an example of how to set tracking constraints in a given
	// logical volume
	//
	// Sets a max step length in the tracker region, with G4StepLimiter

	G4double maxStep = 0.5 * chamberWidth;
	fStepLimit = new G4UserLimits(maxStep);
	trackerLV->SetUserLimits(fStepLimit);

	/// Set additional contraints on the track, with G4UserSpecialCuts
	///
	/// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
	/// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
	///                                           maxLength,
	///                                           maxTime,
	///                                           minEkin));

	// Always return the physical world

	return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::ConstructSDandField()
{
	// Sensitive detectors

	G4String trackerChamberSDname = "B2/TrackerChamberSD";
	B2TrackerSD* aTrackerSD = new B2TrackerSD(trackerChamberSDname,
		"TrackerHitsCollection");
	G4SDManager::GetSDMpointer()->AddNewDetector(aTrackerSD);
	// Setting aTrackerSD to all logical volumes with the same name 
	// of "Chamber_LV".
	SetSensitiveDetector("Chamber_LV", aTrackerSD, true);

	// Create global magnetic field messenger.
	// Uniform magnetic field is then created automatically if
	// the field value is not zero.
	G4ThreeVector fieldValue = G4ThreeVector();
	fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
	fMagFieldMessenger->SetVerboseLevel(1);

	// Register the field messenger for deleting
	G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetTargetMaterial(G4String materialName)
{
	G4NistManager* nistManager = G4NistManager::Instance();

	G4Material* pttoMaterial =
		nistManager->FindOrBuildMaterial(materialName);

	if (fTargetMaterial != pttoMaterial) {
		if (pttoMaterial) {
			fTargetMaterial = pttoMaterial;
			if (fLogicTarget) fLogicTarget->SetMaterial(fTargetMaterial);
			G4cout
				<< G4endl
				<< "----> The target is made of " << materialName << G4endl;
		}
		else {
			G4cout
				<< G4endl
				<< "-->  WARNING from SetTargetMaterial : "
				<< materialName << " not found" << G4endl;
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetChamberMaterial(G4String materialName)
{
	G4NistManager* nistManager = G4NistManager::Instance();

	G4Material* pttoMaterial =
		nistManager->FindOrBuildMaterial(materialName);

	if (fChamberMaterial != pttoMaterial) {
		if (pttoMaterial) {
			fChamberMaterial = pttoMaterial;
			for (G4int copyNo = 0; copyNo < fNbOfChambers; copyNo++) {
				if (fLogicChamber[copyNo]) fLogicChamber[copyNo]->
					SetMaterial(fChamberMaterial);
			}
			G4cout
				<< G4endl
				<< "----> The chambers are made of " << materialName << G4endl;
		}
		else {
			G4cout
				<< G4endl
				<< "-->  WARNING from SetChamberMaterial : "
				<< materialName << " not found" << G4endl;
		}
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetMaxStep(G4double maxStep)
{
	if ((fStepLimit) && (maxStep > 0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2aDetectorConstruction::SetCheckOverlaps(G4bool checkOverlaps)
{
	fCheckOverlaps = checkOverlaps;
}
