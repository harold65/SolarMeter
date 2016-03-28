/*
  Flash.h - An Arduino library for flash-based (ROM) data collections.
  Copyright (C) 2009 Mikal Hart
  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef Flash_h
#define Flash_h

#include <avr/pgmspace.h>
#include <Arduino.h>

#define FLASH_LIBRARY_VERSION 4

#ifndef ARDUINO_STREAMING
#define ARDUINO_STREAMING

template<class T> 
inline Print &operator <<(Print &stream, T arg) 
{ stream.print(arg); return stream; }

enum _EndLineCode { endl };

inline Print &operator <<(Print &stream, _EndLineCode arg) 
{ stream.println(); return stream; }

#endif

#endif