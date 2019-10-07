#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Parameter.hh"
#include "globals.hh"

class G4Run;

class RunAction : public G4UserRunAction
{
public:
	RunAction();
	virtual ~RunAction();

	virtual void BeginOfRunAction(const G4Run*);
	virtual void   EndOfRunAction(const G4Run*);

	void AddEdep(G4double edep);

private:
	G4Parameter<G4double> fEdep;
	G4Parameter<G4double> fEdep2;
};

#endif
