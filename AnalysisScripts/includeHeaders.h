//Standard headers:
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;

//Custom headers:
#include "Setup/CustomFunctions.h"
#include "Setup/ProduceGenCollection.h"
#include "Setup/ProduceRecoCollection.h"
#include "Setup/json_setup.h"
#include "Corrections/calculateCorrections.h"
#include "Corrections/TriggerEfficiency.h"
#include "Corrections/loadJSONweights.h"
