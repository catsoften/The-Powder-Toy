#ifndef CIRCUITS_FRAMEWORK_H
#define CIRCUITS_FRAMEWORK_H

#include "simulation/CoordStack.h"
#include "simulation/ElementCommon.h"
#include "circuit_core.h"
#include <vector>

class Circuits;

bool allow_conduction(int totype, int fromtype);
bool can_be_skeleton(int i, Simulation * sim);
bool can_be_node(int i, Simulation * sim);

coord_vec floodfill(Simulation *sim, int x, int y);
coord_vec coord_vec_to_skeleton(Simulation *sim, const coord_vec &floodfill);

// Type classifications
inline bool is_positive_terminal(int type) {
    return type == PT_PSCN || type == PT_COPR; }
inline bool is_negative_terminal(int type) {
    return type == PT_NSCN || type == PT_ZINC; }
inline bool is_terminal(int type) {
    return is_positive_terminal(type) || is_negative_terminal(type); }
inline bool has_negative_resistance(int type) {
    return type == PT_PLSM || type == PT_NBLE || type == PT_HELM || type == PT_NEON; }
inline bool is_dynamic_resistor(int type) {
    return has_negative_resistance(type) || type == PT_TRST || type == PT_NTCT || type == PT_PTCT ||
        type == PT_PQRT || type == PT_QRTZ || type == PT_MERC || type == PT_CRBN; }
inline bool is_dynamic_particle(int type) {
    return type == PT_PTCT || type == PT_NTCT || type == PT_SWCH || type == PT_CAPR || type == PT_INDC; }
inline bool is_chip(int type) { // Particle outputs direct voltage
    return type == PT_LOGC; }
inline bool is_voltage_source(int type) {
    return type == PT_VOLT || type == PT_CAPR; }
inline bool is_integration_particle(int type) { // Particle does numeric integration
    return type == PT_CAPR || type == PT_INDC; }
#endif

