// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#ifndef DTCSVREADER_H
#define DTCSVREADER_H

#include <QTextStream>
#include <memory>

#include <stdexcept>

namespace dtree {
class DecisionTree;
}

class DtGraphicsScene;

class DtCsvReader
{
public:
    static const int VERSION;

    DtCsvReader();
    bool readTree (QTextStream &stream, std::shared_ptr<dtree::DecisionTree> tree, DtGraphicsScene *scene);
};

#endif // DTCSVREADER_H
