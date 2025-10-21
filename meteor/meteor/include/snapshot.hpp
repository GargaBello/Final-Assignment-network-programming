//snapshot.hpp

#pragma once

#include "network.hpp"
#include "client_controller.hpp"
#include "raylib.h"

namespace meteor {
	struct snapshot {
		
		std::vector<Client> m_clients;
	};
}