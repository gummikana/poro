#ifndef INC_CCHECKSUM_H
#define INC_CCHECKSUM_H

#include <string>

namespace ceng
{

std::string GenerateCheckSumForStringData( const std::string& stringdata, int dept = 32 );
std::string GenerateCheckSumForFile( const std::string& filename, int dept = 32 );

}

#endif
