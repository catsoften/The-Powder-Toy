#include "QuickOptions.h"

#include "GameModel.h"
#include "GameController.h"

#include "simulation/Simulation.h"

SandEffectOption::SandEffectOption(GameModel * m):
QuickOption("P", "Sand effect", m, Toggle)
{

}
bool SandEffectOption::GetToggle()
{
	return m->GetSimulation()->pretty_powder;
}
void SandEffectOption::perform()
{
	m->GetSimulation()->pretty_powder = !m->GetSimulation()->pretty_powder;
}



DrawGravOption::DrawGravOption(GameModel * m):
QuickOption("G", "Draw gravity field \bg(ctrl+g)", m, Toggle)
{

}
bool DrawGravOption::GetToggle()
{
	return m->GetGravityGrid();
}
void DrawGravOption::perform()
{
	m->ShowGravityGrid(!m->GetGravityGrid());
}



DecorationsOption::DecorationsOption(GameModel * m):
QuickOption("D", "Draw decorations \bg(ctrl+b)", m, Toggle)
{

}
bool DecorationsOption::GetToggle()
{
	return m->GetDecoration();
}
void DecorationsOption::perform()
{
	m->SetDecoration(!m->GetDecoration());
}



NGravityOption::NGravityOption(GameModel * m):
QuickOption("N", "Newtonian Gravity \bg(n)", m, Toggle)
{

}
bool NGravityOption::GetToggle()
{
	return m->GetNewtonianGrvity();
}
void NGravityOption::perform()
{
	m->SetNewtonianGravity(!m->GetNewtonianGrvity());
}



AHeatOption::AHeatOption(GameModel * m):
QuickOption("A", "Ambient heat \bg(u)", m, Toggle)
{

}
bool AHeatOption::GetToggle()
{
	return m->GetAHeatEnable();
}
void AHeatOption::perform()
{
	m->SetAHeatEnable(!m->GetAHeatEnable());
}



ConsoleShowOption::ConsoleShowOption(GameModel * m, GameController * c_):
QuickOption("C", "Show Console \bg(~)", m, Toggle)
{
	c = c_;
}
bool ConsoleShowOption::GetToggle()
{
	return 0;
}
void ConsoleShowOption::perform()
{
	c->ShowConsole();
}


TimeDilationOption::TimeDilationOption(GameModel * m):
QuickOption("T", "Draw Time Dilation", m, Toggle) {}
bool TimeDilationOption::GetToggle() {
	return m->GetTimeDilation();
}
void TimeDilationOption::perform() {
	m->ShowTimeDilation(!m->GetTimeDilation());
}

EMFieldOption::EMFieldOption(GameModel * m):
QuickOption("M", "Maxwellian Electromagnetism", m, Toggle) {}
bool EMFieldOption::GetToggle() {
	return m->GetEMEnabled();
}
void EMFieldOption::perform() {
	m->SetEMEnabled(!m->GetEMEnabled());
}

EMFieldElectricOption::EMFieldElectricOption(GameModel * m):
QuickOption("E", "Draw electric field", m, Toggle) {}
bool EMFieldElectricOption::GetToggle() {
	return m->GetElectricField();
}
void EMFieldElectricOption::perform() {
	m->ShowElectricField(!m->GetElectricField());
}

EMFieldMagneticOption::EMFieldMagneticOption(GameModel * m):
QuickOption("M", "Draw magnetic field", m, Toggle) {}
bool EMFieldMagneticOption::GetToggle() {
	return m->GetMagneticField();
}
void EMFieldMagneticOption::perform() {
	m->ShowMagneticField(!m->GetMagneticField());
}


