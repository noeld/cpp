//
// Created by Arnold Müller on 18.02.17.
//

#ifndef CLIONTEST_PROPERTIES_H
#define CLIONTEST_PROPERTIES_H

#include <string>

class Properties {
public:
    explicit Properties(const std::string configname = "default");
    virtual ~Properties();

    bool ReadConfigFile(const std::string& filename, const std::string& configname);
    bool WriteConfigFile(const std::string& filename);

    unsigned simulation_intervall { 50 };
    double   simulation_f { 1.0 };
    double   universe_g { 6.6740831e-11 * 100e8 };
    double   universe_collision_joindist { 0.2 };
    double   universe_collision_k { 0.5 };
    unsigned generate_n { 1000 };
    double   generate_speed_max { 5 };
    double   generate_border_part { 0.05 };
    double   generate_mass_min { 50 };
    double   generate_mass_max { 10000 };
    double   generate_mass_alpha { 0.1 };
    double   generate_mass_beta  { 9.0 };

    std::string configname_;
    static const std::string filename_;
private:

    bool ConfigFileExists() const;
};

class PropertiesReaderWriter {
public:
    virtual void ReadProperties(const Properties&) =0;
    virtual void WriteProperties(Properties&) =0;

};


#endif //CLIONTEST_PROPERTIES_H
