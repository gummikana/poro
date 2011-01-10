#include "ctimer_impl.h"

#include <sdl.h>

namespace ceng {
namespace impl {


types::ticks GetTime()
{
	return (types::ticks)SDL_GetTicks();
}

} // end of namespace impl
} // end of namespace ceng