
#include <iostream>
#include <fstream>

#ifdef JSON_EXPORT
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#endif

#include <CherenkovDetector.h>

namespace IRT2 {

// -------------------------------------------------------------------------------------
//
// Cut'n'paste from https://stackoverflow.com/questions/40029100/c-how-to-write-back-json-into-file;
//// Source - https://stackoverflow.com
// Posted by Danie
// Retrieved 2025-12-21, License - CC BY-SA 4.0

#ifdef JSON_EXPORT
template<typename ITERATOR_TYPE>
void print_element(ITERATOR_TYPE it, unsigned int depth, std::ostream& output_stream);
void print_array(nlohmann::json::array_t arr, unsigned int depth, std::ostream& output_stream);
void print_object(nlohmann::json obj, unsigned int depth, std::ostream& output_stream);

void export_json(nlohmann::json json_file, std::ostream& output_stream) {
    output_stream << "{\n";

    for (nlohmann::json::iterator iter = json_file.begin();;) {
        output_stream << '\t' << '"' << iter.key() << "\": ";
        
        print_element(iter, 1, output_stream);

        if (++iter != json_file.end()) {
            output_stream << ',' << '\n';
        } else {
            break;
        }
    }

    output_stream << "\n}";
}

// Adjust it in a way arrays are printed in a single line;
void print_array(nlohmann::json::array_t arr, unsigned int depth, std::ostream& output_stream) {
    if (!arr.size())
        return;

    //std::string indent = std::string(depth, '\t');
    for (nlohmann::json::array_t::iterator iter = arr.begin();;) {
      //output_stream << '\n' << indent << '\t';
      output_stream << ' ';
        print_element(iter, depth + 1, output_stream);

        if (++iter != arr.end()) {
            output_stream << ',';
        } else {
            break;
        }
    }
    //output_stream << '\n' << indent;
}

void print_object(nlohmann::json obj, unsigned int depth, std::ostream& output_stream) {
    if (!obj.size())
        return;

    std::string indent = std::string(depth, '\t');
    for (nlohmann::json::iterator iter = obj.begin();;) {
        output_stream << '\n' << indent << '\t' << '"' << iter.key() << "\": ";
        print_element(iter, depth + 1, output_stream);

        if (++iter != obj.end()) {
            output_stream << ',';
        } else {
            break;
        }
    }
    output_stream << '\n' << indent;
}

template<typename ITERATOR_TYPE>
void print_element(ITERATOR_TYPE iter, unsigned int depth, std::ostream& output_stream) {
    switch (iter->type())
    {
    case nlohmann::json::value_t::object:
      output_stream << '{';
      print_object(nlohmann::json(*iter), depth, output_stream);
      output_stream << '}';
      break;
      
    case nlohmann::json::value_t::array:     
      output_stream << '[';
      print_array(nlohmann::json::array_t(*iter), depth, output_stream);
      output_stream << ']';
      break;
      
    default:
      output_stream << *iter;
      break;
    }
}
#endif

// -------------------------------------------------------------------------------------

void CherenkovDetector::ExportJsonFormatCalibrations(const char *fname)
{
#ifdef JSON_EXPORT
  std::ofstream ofile(fname);
  
  if (!ofile.is_open())  {
    std::cout << "\n Failed to open output file";
  } else {
    nlohmann::json data;
    auto &r1data = data["Radiators"] = nlohmann::json::object_t();
    for(auto [name,radiator]: Radiators()) {
      //auto radiator = rptr.second;
      auto &r2data = r1data[/*rptr.first*/name] = nlohmann::json::object_t();
      
      auto &r3data = r2data["theta-bins"] = nlohmann::json::object_t();
      for(unsigned iq=0; iq<radiator->m_Calibrations.size(); iq++) {
	auto &calib = radiator->m_Calibrations[iq];

	if (calib.m_Stat) {
	  std::vector<std::string> ri_strings;
	  TString bin, stat, zvtx, offset, sigma, ristr;
	  bin.Form("%02d", iq);
	  stat.Form  (  "%6d",      calib.m_Stat);
	  zvtx.Form  ("%7.1f",      calib.m_AverageZvtx);
	  // '1000': prefer to have them in a readable [mrad] format;
	  offset.Form("%7.2f", 1000.*calib.m_Coffset);
	  sigma.Form ("%7.2f", 1000.*calib.m_Csigma);
	  //r3data[bin.Data()] = {stat.Data(), zvtx.Data(), offset.Data(), sigma.Data()};
	  ri_strings.push_back(stat.Data());
	  ri_strings.push_back(zvtx.Data());
	  ri_strings.push_back(offset.Data());
	  ri_strings.push_back(sigma.Data());
	    
	  for(auto ri: calib.m_AverageRefractiveIndices) {
	    ristr.Form("%8.6f", ri);
	    ri_strings.push_back(ristr.Data());
	  } //for ri
	    
	  //r2data["refractive-indices"] = ri_strings;
	  r3data[bin.Data()] = ri_strings;
	} //if
      } //for calib
    } //for rptr
    
    export_json(data, ofile);
    ofile.close();
  } //if
#endif
} // CherenkovDetector::ExportJsonFormatCalibrations()

} // namespace IRT2

// -------------------------------------------------------------------------------------
