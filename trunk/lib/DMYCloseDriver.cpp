/*
* Copyright (C) 2007, A. Giannetti
*
* This file is part of Hudson.
*
* Hudson is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Hudson is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Hudson.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"

// STDLIB
#include <cstdlib>

// Boost
#include <boost/tokenizer.hpp>

// Series
#include "DMYCloseDriver.hpp"

using namespace std;
using namespace boost;
using namespace boost::gregorian;


Series::DMYCloseDriver::DMYCloseDriver(void):
  _linenum(0)
{
}


Series::DMYCloseDriver::~DMYCloseDriver(void)
{
  if( _infile.is_open() )
	  _infile.close();
}


bool Series::DMYCloseDriver::open(const std::string& filename)
{
  // Check if another file was open previously
  if( _infile.is_open() ) {
	  _infile.close();
	  _linenum = 0;
  }

  _infile.open(filename.c_str());
  if( _infile.is_open() == false )
	return false;

  // First line is header line
  getline(_infile, _line);
  _linenum = 1;

  return _infile.good();
}


void Series::DMYCloseDriver::close(void)
{
  if( _infile.is_open() )
	  _infile.close();

  _infile.clear();

  _linenum = 0;
}


bool Series::DMYCloseDriver::next(DayPrice& dp) throw(Series::DriverException)
{
  if( _infile.eof() )
  	return false;

  typedef tokenizer<boost::char_separator<char> > TokensRdr;
  getline(_infile, _line);
  ++_linenum;

  dp.adjclose = 0;
  dp.close = 0;
  dp.volume = 0;
  dp.high = 0;
  dp.low = 0;
  dp.key = date(); // not_a_date_time
  dp.open = 0;

  // Get line fields
  int i = 0;
  TokensRdr tok(_line, boost::char_separator<char>(" ,\t\n\r"));
  for( TokensRdr::iterator iter = tok.begin(); iter != tok.end(); ++iter, ++i ) {

	  string field = *iter;		// extract line field (.csv file)

	  switch( i ) {

	    case DATE: {
	      dp.key = from_uk_string(field);
	      if( dp.key.is_not_a_date() ) {
		      stringstream ss;
		      ss << "Invalid key at line " << _linenum;
		      throw DriverException(ss.str());
	      }
	    }
	    break;

	    case CLOSE:
	      dp.high = dp.low = dp.open = dp.close = dp.adjclose = atof(field.c_str());
	      break;

	    default: {
		    stringstream ss;
		    ss << "Unknown field at line " << _linenum;
		    throw DriverException(ss.str());
	    }
	  } // token type switch
  } // for each token in line

  return true;
}


bool Series::DMYCloseDriver::eof(void)
{
  return _infile.eof();
}

