#ifndef CIRCUIT_BRANCH_CLASS
#define CIRCUIT_BRANCH_CLASS

#include "simulation/ElementCommon.h"
#include "simulation/circuits/circuit_core.h"

#include <vector>
#include <unordered_map>
#include <iostream>

class Circuit;

// A line between 2 nodes. I have no idea what they're actually called
// so I'll call them branches (or maybe edges)
class Branch {
public:
    /**
     * Resistance is base resistance (only resistors)
     * Effective resistance is dynamic, base is static
     * 
     * Notes on polarity:
     * Voltage: going from node1 to node2 drops by this voltage, ie
     *   NODE1 ----- + | - ------ NODE2
     * is positive voltage
     * 
     * Current: flows from node1 to node2 (positive)
     *   NODE1 -----> NODE2 (positive)
     * 
     * Dioide: going from node1 to node2 is positive_diode
     *   NODE1 ----- PSCN NSCN ------ NODE2
     */

    const int node1, node2;
    const std::vector<int> ids;
    const std::vector<int> rspk_ids;
    const std::vector<int> switches;
    const std::vector<int> dynamic_resistors;
    double resistance, voltage_gain, current_gain, base_resistance, current=0.0;
    const int diode; // 0 = no diode, 1 = positive, -1 = negative
    const int node1_id, node2_id;
    double V1, V2;
    bool recompute_switches = true;
    
    // Divergence checks
    double SS_voltage = std::numeric_limits<double>::max(), SS_current = std::numeric_limits<double>::max();

    Branch(int node1, int node2, const std::vector<int> &ids, 
            const std::vector<int> &rspk_ids, const std::vector<int> &switch_ids, const std::vector<int> &dynamic_resistors,
            double resistance, double voltage_gain, double current_gain, int diode, int id1, int id2) :
        node1(node1), node2(node2), ids(ids), rspk_ids(rspk_ids), switches(switch_ids), dynamic_resistors(dynamic_resistors),
        resistance(resistance), voltage_gain(voltage_gain), current_gain(current_gain), base_resistance(resistance), diode(diode),
        node1_id(id1), node2_id(id2) {}
    
    void setSpecialType(bool isCapacitor, bool isInductor, bool isChip);
    void print();
    void setToSteadyState();

    void computeDynamicResistances(Simulation * sim, Circuit * c);
    void computeDynamicVoltages(Simulation * sim, Circuit * c);
    void computeDynamicCurrents(Simulation * sim, Circuit * c);

    bool obeysOhmsLaw()    { return obeys_ohms_law; };
    bool isVoltageSource() { return voltage_gain; }

    bool isInductor()      { return branch_type == INDUCTOR; }
    bool isCapacitor()     { return branch_type == CAPACITOR; }
    bool isChip()          { return branch_type == CHIP; }
    bool isDiode()         { return branch_type == DIODE; }
private:
    bool obeys_ohms_law = true;
    bool switches_on = false;
    bool switchesOn(Simulation * sim);

    enum TYPE { RESISTOR, CAPACITOR, INDUCTOR, CHIP, DIODE, VOLTAGE_SOURCE };
    TYPE branch_type = RESISTOR;    
};

#endif