#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
	:G4VUserPrimaryGeneratorAction(),
	fParticleGun(0)
{
	G4int n_particle = 1;
	fParticleGun = new G4ParticleGun(n_particle);

	//Particle type
	G4particleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle
		= particleTable->FindParticle(particleName = "gamma");
	fParticleGun->SetParticleDefinition(particle);

	//Direction
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0, 0);

	//Postion
	fParticleGun->SetParticlePosition(G4ThreeVector(-10 * cm, 0, 0));

	//Energy
	fParticleGun->SetParticleEnergy(6.1 * MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
	{
		delete fParticleGun;
	}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	fParticleGun->GeneratePrimaryVertex(anEvent);
}
