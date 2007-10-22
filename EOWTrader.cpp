/*
* Copyright (C) 2007, Alberto Giannetti
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

#include "EOWTrader.hpp"

using namespace std;
using namespace boost::gregorian;


EOWTrader::EOWTrader(const DB& db):
  _db(db),
  _invested_days(0)
{
}


void EOWTrader::run(unsigned entry_offset, char entry_oc, unsigned exit_offset, char exit_oc) throw(TraderException)
{
  assert(entry_offset >= exit_offset);

  _invested_days = days(0);
  date my_first_entry;
  date my_last_exit;

  for( week_iterator witer(_db.begin()->first); witer < _db.rbegin()->first; ++witer ) {

	DB::const_iterator last_tradeday_iter = _db.last_in_week((*witer).year(), (*witer).month(), (*witer).day());
	if( last_tradeday_iter == _db.end() ) {
	  cerr << "Can't find EOW at date " << (*witer) << endl;
	  continue;
	}

	DB::const_iterator entry_iter = _db.before(last_tradeday_iter->first, entry_offset);
	if( entry_iter == _db.end() ) {
	  cerr << "Can't find actual entry date for EOW " << last_tradeday_iter->first << endl;
	  continue;
	}

	DB::const_iterator exit_iter = _db.before(last_tradeday_iter->first, exit_offset);
	if( exit_iter == _db.end() ) {
	  cerr << "Can't find actual exit date for EOW " << last_tradeday_iter->first << endl;
	  continue;
	}

	try {

	  double entry_price = (toupper(entry_oc) == 'O' ? entry_iter->second.open : entry_iter->second.close);
	  double exit_price  = (toupper(exit_oc) == 'O' ? exit_iter->second.open : exit_iter->second.close);

	  Position::ID id = buy("EOW", entry_iter->first, entry_price);
	  close(id, exit_iter->first, exit_price);

	} catch( TraderException& te ) {
	  cerr << "Trader error: " << te.what() << endl;
	  continue;

	} catch( std::exception& e ) {
	  cerr << "Standard exception: " << e.what() << endl;
	  continue;
	}

	date_duration dpl = date_period(entry_iter->first, exit_iter->first).length();
	if( dpl == days(0) )
	  dpl = days(1); // some open/close same-day trade...
	_invested_days = _invested_days + dpl;

	if( my_first_entry.is_not_a_date() ) my_first_entry = entry_iter->first;
	my_last_exit = exit_iter->first;

  }	// for each week in price database

  _first_entry = my_first_entry;
  _last_exit = my_last_exit;
}
