/* Copyright (c) IAIK, Graz University of Technology, 2015.
 * All rights reserved.
 * Contact: http://opensource.iaik.tugraz.at
 * 
 * This file is part of the Secure Block Device Library.
 * 
 * Commercial License Usage
 * Licensees holding valid commercial licenses may use this file in
 * accordance with the commercial license agreement provided with the
 * Software or, alternatively, in accordance with the terms contained in
 * a written agreement between you and SIC. For further information
 * contact us at http://opensource.iaik.tugraz.at.
 * 
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License as published by the Free Software Foundation version 2.
 * 
 * The Secure Block Device Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with the Secure Block Device Library. If not, see <http://www.gnu.org/licenses/>.
 */
///
/// \file
/// \brief Memory buffer for reading and writing primitive data types to an
/// untyped memory region (implementation).
///
#include "sbdi_buffer.h"
#include "sbdi_err.h"

#include <assert.h>

/*!
 * \brief tests if the given buffer pointer points to a valid SBDI buffer
 * @param buf[in] a pointer to the SBDI buffer to validate
 * @return true if the buffer is valid; false otherwise
 */
static int sbdi_buffer_is_valid(const sbdi_buffer_t *buf)
{
  return (buf && buf->buffer && buf->pos < buf->length);
}

//----------------------------------------------------------------------
void sbdi_buffer_init(sbdi_buffer_t *buf, uint8_t *buffer, const size_t length)
{
  assert(buf && buffer && length > 0);
  buf->buffer = buffer;
  buf->length = length;
  buf->pos = 0;
}

//----------------------------------------------------------------------
void sbdi_buffer_reset(sbdi_buffer_t *buf)
{
  assert(buf && buf->buffer && buf->pos <= buf->length);
  buf->pos = 0;
}

//----------------------------------------------------------------------
uint8_t *sbdi_buffer_get_cptr(const sbdi_buffer_t *buf)
{
  assert(sbdi_buffer_is_valid(buf));
  return &buf->buffer[buf->pos];
}

//----------------------------------------------------------------------
uint8_t *sbdi_buffer_get_cptr_off(const sbdi_buffer_t *buf, const uint32_t off)
{
  assert(buf && buf->buffer && off < buf->length);
  return &buf->buffer[off];
}

//----------------------------------------------------------------------
void sbdi_buffer_add_pos(sbdi_buffer_t *buf, const uint32_t add)
{
  assert(sbdi_buffer_is_valid(buf));
  // Overflow protection
  assert(buf->pos <= (UINT32_MAX - add));
  assert((buf->pos + add) < buf->length);
  buf->pos += add;
}

//----------------------------------------------------------------------
void sbdi_buffer_write_uint8_t(sbdi_buffer_t *buf, uint8_t value)
{
  assert(sbdi_buffer_is_valid(buf));
  // Overflow protection
  assert(buf->pos != UINT32_MAX);
  buf->buffer[buf->pos] = value;
  buf->pos += 1;
}

//----------------------------------------------------------------------
void sbdi_buffer_write_uint16_t(sbdi_buffer_t *buf, uint16_t value)
{
  assert(sbdi_buffer_is_valid(buf));
  assert(buf->pos <= buf->length - 2);
  sbdi_buffer_write_uint8_t(buf, (value >> 8) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, value & 0xFF);
}

//----------------------------------------------------------------------
void sbdi_buffer_write_uint32_t(sbdi_buffer_t *buf, uint32_t value)
{
  assert(sbdi_buffer_is_valid(buf));
  assert(buf->pos <= buf->length - 4);
  sbdi_buffer_write_uint8_t(buf, (value >> 24) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 16) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 8) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value) & 0xFF);
}

//----------------------------------------------------------------------
void sbdi_buffer_write_uint64_t(sbdi_buffer_t *buf, const uint64_t value)
{
  assert(sbdi_buffer_is_valid(buf));
  assert(buf->pos <= buf->length - 8);
  sbdi_buffer_write_uint8_t(buf, (value >> 56) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 48) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 40) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 32) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 24) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 16) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value >> 8) & 0xFF);
  sbdi_buffer_write_uint8_t(buf, (value) & 0xFF);
}

//----------------------------------------------------------------------
void sbdi_buffer_write_bytes(sbdi_buffer_t *buf, const uint8_t *src,
    const size_t length)
{
  assert(sbdi_buffer_is_valid(buf));
  // Overflow protection
  assert(buf->pos <= (UINT32_MAX - length));
  assert(buf->pos <= buf->length - length);
  for (int i = 0; i < length; ++i) {
    sbdi_buffer_write_uint8_t(buf, src[i]);
  }
}

//----------------------------------------------------------------------
void sbdi_buffer_write_ctr_128b(sbdi_buffer_t *buf, const sbdi_ctr_128b_t *ctr)
{
  assert(sbdi_buffer_is_valid(buf));
  sbdi_buffer_write_uint64_t(buf, ctr->hi);
  sbdi_buffer_write_uint64_t(buf, ctr->lo);
}

//----------------------------------------------------------------------
uint8_t sbdi_buffer_read_uint8_t(sbdi_buffer_t *buf)
{
  assert(sbdi_buffer_is_valid(buf));
  // Overflow protection
  assert(buf->pos != UINT32_MAX);
  uint8_t value = buf->buffer[buf->pos];
  buf->pos += 1;
  return value;
}

//----------------------------------------------------------------------
uint16_t sbdi_buffer_read_uint16_t(sbdi_buffer_t *buf)
{
  assert(sbdi_buffer_is_valid(buf));
  assert(buf->pos <= buf->length - 2);
  uint16_t value = 0;
  value |= ((uint16_t) sbdi_buffer_read_uint8_t(buf)) << 8;
  value |= ((uint16_t) sbdi_buffer_read_uint8_t(buf)) << 0;
  return value;
}

//----------------------------------------------------------------------
uint32_t sbdi_buffer_read_uint32_t(sbdi_buffer_t *buf)
{
  assert(sbdi_buffer_is_valid(buf));
  assert(buf->pos <= buf->length - 4);
  uint32_t value = 0;
  value |= ((uint32_t) sbdi_buffer_read_uint8_t(buf)) << 24;
  value |= ((uint32_t) sbdi_buffer_read_uint8_t(buf)) << 16;
  value |= ((uint32_t) sbdi_buffer_read_uint8_t(buf)) << 8;
  value |= ((uint32_t) sbdi_buffer_read_uint8_t(buf)) << 0;
  return value;
}

//----------------------------------------------------------------------
uint64_t sbdi_buffer_read_uint64_t(sbdi_buffer_t *buf)
{
  assert(sbdi_buffer_is_valid(buf));
  assert(buf->pos <= buf->length - 8);
  uint64_t value = 0;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 56;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 48;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 40;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 32;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 24;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 16;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 8;
  value |= ((uint64_t) sbdi_buffer_read_uint8_t(buf)) << 0;
  return value;
}

//----------------------------------------------------------------------
void sbdi_buffer_read_bytes(sbdi_buffer_t *buf, uint8_t *dest,
    const size_t length)
{
  assert(sbdi_buffer_is_valid(buf));
  // Overflow protection
  assert(buf->pos <= (UINT32_MAX - length));
  assert(buf->pos <= buf->length - length);
  for (int i = 0; i < length; ++i) {
    dest[i] = sbdi_buffer_read_uint8_t(buf);
  }
}

//----------------------------------------------------------------------
sbdi_error_t sbdi_buffer_read_ctr_128b(sbdi_buffer_t *buf, sbdi_ctr_128b_t *ctr)
{
  assert(sbdi_buffer_is_valid(buf));
  uint64_t hi = sbdi_buffer_read_uint64_t(buf);
  uint64_t lo = sbdi_buffer_read_uint64_t(buf);
  return sbdi_ctr_128b_init(ctr, hi, lo);
}
