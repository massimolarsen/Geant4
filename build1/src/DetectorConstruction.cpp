#include "DetectorConstruction.h"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SensitiveDetector.hh"


DetectorConstruction::DetectorConstruction()
	: G4VUserDetectorConstruction(),
	  fScoringVolume(0)
{ }


DetectorConstruction::~DetectorConstruction()
{ }

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	G4NistManager* nist = G4NistManager::Instance();

	// World
	G4double world_sizeXYZ = 50 * cm;
	G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

	G4Box* solidWorld =
		new G4Box(
			"World",
			world_sizeXYZ,
			world_sizeXYZ,
			world_sizeXYZ
		);

	G4LogicalVolume* logicWorld =
		new G4LogicalVolume(
			solidWorld,
			world_mat,
			"World"
		);

	G4PhysicalVolume* physWorld =
		new G4PVPlacement(
			0,
			G4ThreeVector(),
			logicWorld,
			"World",
			0,
			false,
			0,
			checkOverlaps
		);


	// Detector volume
	G4double det_sizeXYZ = 3 * cm;
	G4Material* det_mat = nist->FindOrBuildMaterial("G4_Ge");
	G4ThreeVector pos = G4ThreeVector(10 * cm, 0, 0);

	G4Box* solidDet =
		new G4Box(
			"Detector",
			det_sizeXYZ,
			det_sizeXYZ,
			det_sizeXYZ
		);

	// Sensitive Detector
	G4SensitiveDetector* sensitive =
		new G4SensitiveDetector();

	G4SDManager* sdman = G4SDManager::GetSDMpointer();
	sdman->AddNewDetector(sensitive);


	G4LogicalVolume* logicDet =
		new G4LogicalVolume(
			solidDet,
			det_mat,
			"Detector",
			0,
			sensitive
		);

	G4PhysicalVolume* physDet =
		new G4PVPlacement(
			0,
			pos,
			logicDet,
			"Detector",
			logicWorld,
			false,
			0,
			checkOverlaps
		);

	fScoringVolume = logicDet;

	return physWorld;
}