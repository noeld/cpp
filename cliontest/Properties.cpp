//
// Created by Arnold Müller on 18.02.17.
//

#include "Properties.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

const std::string Properties::filename_ { "simulation.json"};

Properties::Properties(const std::string configname) : configname_(configname)
{
    ReadConfigFile(Properties::filename_, configname_);
}

Properties::~Properties() {
    WriteConfigFile(Properties::filename_);
}

bool Properties::ReadConfigFile(const std::string& filename, const std::string& configname) {
    using boost::property_tree::iptree;
    if (ConfigFileExists()) {
        iptree pt;
        boost::property_tree::read_json(Properties::filename_, pt);
        iptree& def = pt.get_child(configname);

        simulation_intervall        = def.get<unsigned>("simulation.intervall", simulation_intervall);
        simulation_f                = def.get<double>("simulation.f", simulation_f);
        universe_g                  = def.get<double>("universe.g", universe_g);
        universe_planet_density     = def.get<double>("universe.planet.density", universe_planet_density);
        universe_collision_joindist = def.get<double>("universe.collision.joindist", universe_collision_joindist);
        universe_collision_k        = def.get<double>("universe.collision.k", universe_collision_k);
        generate_n                  = def.get<unsigned>("generate.n", generate_n);
        generate_speed_min          = def.get<double>("generate.speed.min", generate_speed_min);
        generate_speed_max          = def.get<double>("generate.speed.max", generate_speed_max);
        generate_border_part        = def.get<double>("generate.border.part", generate_border_part);
        generate_mass_min           = def.get<double>("generate.mass.min", generate_mass_min);
        generate_mass_max           = def.get<double>("generate.mass.max", generate_mass_max);
        generate_mass_alpha         = def.get<double>("generate.mass.alpha", generate_mass_alpha);
        generate_mass_beta          = def.get<double>("generate.mass.beta", generate_mass_beta);
        generate_pos_xmax           = def.get<double>("generate.pos.xmax", generate_pos_xmax);
        generate_pos_ymax           = def.get<double>("generate.pos.ymax", generate_pos_xmax);
        view_forcefield_show        = def.get<bool>("view.forcefield.show", view_forcefield_show);
        view_forcefield_columns     = def.get<unsigned>("view.forcefield.columns", view_forcefield_columns);
        return true;
    }
    return false;
}

bool Properties::ConfigFileExists() const {
    return boost::filesystem::exists(boost::filesystem::path(Properties::filename_));
}

bool Properties::WriteConfigFile(const std::string &filename) {
    using boost::property_tree::iptree;
    iptree def;
    if (ConfigFileExists()) {
        boost::property_tree::read_json(Properties::filename_, def);
    }

    def.put<unsigned>(configname_ + ".simulation.intervall", simulation_intervall);
    def.put<double>(configname_ + ".simulation.f", simulation_f);
    def.put<double>(configname_ + ".universe.g", universe_g);
    def.put<double>(configname_ + ".universe.planet.density", universe_planet_density);
    def.put<double>(configname_ + ".universe.collision.joindist", universe_collision_joindist);
    def.put<double>(configname_ + ".universe.collision.k", universe_collision_k);
    def.put<unsigned>(configname_ + ".generate.n", generate_n);
    def.put<double>(configname_ + ".generate.speed.min", generate_speed_min);
    def.put<double>(configname_ + ".generate.speed.max", generate_speed_max);
    def.put<double>(configname_ + ".generate.border.part", generate_border_part);
    def.put<double>(configname_ + ".generate.mass.min", generate_mass_min);
    def.put<double>(configname_ + ".generate.mass.max", generate_mass_max);
    def.put<double>(configname_ + ".generate.mass.alpha", generate_mass_alpha);
    def.put<double>(configname_ + ".generate.mass.beta", generate_mass_beta);
    def.put<double>(configname_ + ".generate.pos.xmax", generate_pos_xmax);
    def.put<double>(configname_ + ".generate.pos.ymax", generate_pos_ymax);
    def.put<bool>(configname_ + ".view.forcefield.show", view_forcefield_show);
    def.put<unsigned>(configname_ + ".view.forcefield.columns", view_forcefield_columns);

    boost::property_tree::write_json(Properties::filename_, def);
    return false;
}

void Properties::AddReaderWriter(PropertiesReaderWriter &p) {
    this->readerwriters_.insert(&p);
}

void Properties::RemoveReaderWriter(PropertiesReaderWriter &p) {
    this->readerwriters_.erase(&p);
}

void Properties::UpdateReaders() const {
    for(auto& r : this->readerwriters_) {
        r->ReadProperties(*this);
    }
}

void Properties::UpdateWriters() {
    for(auto& w : this->readerwriters_) {
        w->WriteProperties(*this);
    }
}
