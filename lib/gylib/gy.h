/*
File:   gy.h
Author: Taylor Robbins
Date:   10\07\2022
Description:
	** Includes all gylib files in the proper order of dependency
*/

#ifndef _GY_H
#define _GY_H

//These two might need to be included earlier
#include "gylib/gy_defines_check.h"
#include "gylib/gy_basic_macros.h"

#include "gylib/gy_std.h"
#include "gylib/gy_assert.h"
#include "gylib/gy_types.h"
#include "gylib/gy_threading.h"
#include "gylib/gy_lookup.h"
#include "gylib/gy_os_defs.h"
#include "gylib/gy_intrinsics.h"
#include "gylib/gy_debug.h"
#include "gylib/gy_random.h"
#include "gylib/gy_noise.h"
#include "gylib/gy_easing.h"
#include "gylib/gy_hash.h"
#include "gylib/gy_memory.h"
#include "gylib/gy_scratch_arenas.h"
#include "gylib/gy_fifo.h"
#include "gylib/gy_unicode.h"
#include "gylib/gy_time.h"
#include "gylib/gy_audio.h"
#include "gylib/gy_string.h"
#include "gylib/gy_string_buffer.h"
#include "gylib/gy_uuid.h"
#include "gylib/gy_stream.h"
#include "gylib/gy_vectors.h"
#include "gylib/gy_directions.h"
#include "gylib/gy_basis_vectors.h"
#include "gylib/gy_quaternions.h"
#include "gylib/gy_rectangles.h"
#include "gylib/gy_cyclic_functions.h"
#include "gylib/gy_math.h"
#include "gylib/gy_matrices.h"
#include "gylib/gy_rays.h"
#include "gylib/gy_primitives.h"
#include "gylib/gy_colors.h"
#include "gylib/gy_standard_colors.h"
#include "gylib/gy_string_builder.h"
#include "gylib/gy_parsing.h"
#include "gylib/gy_triangulation.h"
#include "gylib/gy_sorting.h"
#include "gylib/gy_linked_list.h"
#include "gylib/gy_variable_array.h"
#include "gylib/gy_id_map.h"
#include "gylib/gy_str_hash_dictionary.h"
#include "gylib/gy_word_tree.h"
#include "gylib/gy_bezier.h"
#include "gylib/gy_bucket_array.h"
#include "gylib/gy_string_fifo.h"
#include "gylib/gy_process_log.h"
#include "gylib/gy_serialization_helpers.h"
#include "gylib/gy_serializable.h"
#include "gylib/gy_common_serializables.h"
#include "gylib/gy_simple_parsers.h"
#include "gylib/gy_cereal.h"
#include "gylib/gy_resource_manifest.h"
#include "gylib/gy_collision.h"

#include "gylib/gy_os.h"

#include "gylib/gy_ip.h"
#include "gylib/gy_socket.h"
#include "gylib/gy_extras.h"
#include "gylib/gy_network_bus.h"

#endif //  _GY_H
