#ifndef INC_TEST_STARTSERVER_H
#define INC_TEST_STARTSERVER_H

#include "../../multiplayer_utils/framework/multiplayer_data.h"

int Test_StartClient( float update_freq, const MultiplayerData& m_data );
int Test_StartServer( float update_freq, const MultiplayerData& m_data );

#endif