/**
 * libkeccak – Keccak-family hashing library
 * 
 * Copyright © 2014  Mattias Andrée (maandree@member.fsf.org)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "generalised-spec.h"


#define have(v)      (spec->v != LIBKECCAK_GENERALISED_SPEC_AUTOMATIC)
#define copy(v)      (spec->v = v)
#define deft(v, dv)  (have(v) ? v : (dv))


/**
 * Convert a `libkeccak_generalised_spec_t` to a `libkeccak_spec_t`
 *
 * If you are interrested in finding errors, you should call
 * `libkeccak_spec_check(output)` if this function returns zero
 * 
 * @param  spec    The generalised input specifications, may be modified
 * @param  output  The specification datastructure to fill in
 * @return         Zero if `spec` is valid, a `LIBKECCAK_GENERALISED_SPEC_ERROR_*` if an error was found
 */
int libkeccak_degeneralise_spec(libkeccak_generalised_spec_t* restrict spec,
				libkeccak_spec_t* restrict output)
{
  long state_size, word_size, capacity, bitrate, output;
  
  if (have(state_size))
    {
      copy(state_size);
      if (state_size <= 0)    return LIBKECCAK_GENERALISED_SPEC_ERROR_STATE_NONPOSITIVE;
      if (state_size > 1600)  return LIBKECCAK_GENERALISED_SPEC_ERROR_STATE_TOO_LARGE;
      if (state_size % 25)    return LIBKECCAK_GENERALISED_SPEC_ERROR_STATE_MOD_25;
    }
  
  if (have(word_size))
    {
      copy(word_size);
      if (word_size <= 0)  return LIBKECCAK_GENERALISED_SPEC_ERROR_WORD_NONPOSITIVE;
      if (word_size > 64)  return LIBKECCAK_GENERALISED_SPEC_ERROR_WORD_TOO_LARGE;
      if (have(state_size) && (state_size != word_size * 25))
	return LIBKECCAK_GENERALISED_SPEC_ERROR_STATE_WORD_INCOHERENCY;
      else if (!have(state_size))
	spec->state_size = 1, state_size = word_size * 25;
    }
  
  if (have(capacity))
    {
      copy(capacity);
      if (capacity <= 0)  return LIBKECCAK_GENERALISED_SPEC_ERROR_CAPACITY_NONPOSITIVE;
      if (capacity & 7)   return LIBKECCAK_GENERALISED_SPEC_ERROR_CAPACITY_MOD_8;
    }
  
  if (have(bitrate))
    {
      copy(bitrate);
      if (bitrate <= 0)  return LIBKECCAK_GENERALISED_SPEC_ERROR_BITRATE_NONPOSITIVE;
      if (bitrate & 7)   return LIBKECCAK_GENERALISED_SPEC_ERROR_BITRATE_MOD_8;
    }
  
  if (have(output))
    {
      copy(output);
      if (output <= 0)  return LIBKECCAK_GENERALISED_SPEC_ERROR_OUTPUT_NONPOSITIVE;
    }
  
  
  if (!have(bitrate) && !have(capacity) && !have(opacity)) /* state_size? */
    {
      state_size = deft(state_size, 1600L);
      output = ((state_size << 5) / 100L + 7L) & ~0x07L;
      bitrate = output << 1;
      capacity = state_size - bitrate;
      output = output >= 8 ? output : 8;
    }
  else if (!have(bitrate) && !have(capacity)) /* have(output) state_size? */
    {
      bitrate = 1024;
      capacity = 1600 - 1024;
      state_size = deft(state_size, bitrate + capacity);
    }
  else if (!have(bitrate)) /* have(capacity) output? state_size? */
    {
      state_size = deft(state_size, 1600L);
      bitrate = state_size - capacity;
      output = deft(output, capacity == 8 ? 8 : (capacity << 1));
    }
  else if (!have(capacity)) /* have(bitrate) output? state_size? */
    {
      state_size = deft(state_size, 1600L);
      capacity = state_size - bitrate;
      output = deft(output, capacity == 8 ? 8 : (capacity << 1));
    }
  else /* have(bitrate) have(capacity) output? state_size? */
    {
      state_size = deft(state_size, bitrate + capacity);
      output = deft(output, capacity == 8 ? 8 : (capacity << 1));
    }
}


#undef deft
#undef copy
#undef have
