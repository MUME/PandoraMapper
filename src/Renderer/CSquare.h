/*
 *  Pandora MUME mapper
 *
 *  Copyright (C) 2000-2009  Azazello
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef CSQUARE_H
#define CSQUARE_H

#include <QVector>
#include <QColor>

class CRoom;

// temporary storage for a billboard text
class Billboard {

public:
	Billboard() {}
	Billboard(double _x, double _y, double _z, QString _text, QColor _col):x(_x),y(_y),z(_z),color(_col),text(_text) {}
	~Billboard() { text.clear(); }

	double 		x;
	double 		y;
	double 		z;
	QColor   	color;
	QString 	text;
};



class CSquare {
public:

    enum SquareTypes {
        Left_Upper = 0,             
        Right_Upper = 1,            
        Left_Lower = 2,
        Right_Lower = 3
    };

//    QList<QScopedPointer< Billboard> > notes;
//    QList<QScopedPointer< Billboard> > doors;

    void clearNotesList();
    void clearDoorsList();


    int 	gllist;
    bool 	rebuild_display_list;

    QList<Billboard *> notesBillboards;
    QList<Billboard *> doorsBillboards;

    /* subsquares */
    CSquare     *subsquares[4];
    /* coordinates of this square's left (upper) and right (lower) points */
    int         leftx, lefty;
    int         rightx, righty;
    int         centerx, centery;
    
    /* amount of rooms in this square, -1 for empty */
    QVector<CRoom *> 	rooms;

    CSquare(int leftx, int lefty, int rightx, int righty);
    CSquare();
    ~CSquare();

    /* mode == SquareType */
    int         getMode(CRoom *room);
    int         getMode(int x, int y);
    bool        toBePassed();
    bool        isInside(CRoom *room);  
    
    void        addSubsquareByMode(int mode);
    void        addRoomByMode(CRoom *room, int mode);
        
    void        add(CRoom *room);
    void        remove(CRoom *room);
    void 		rebuildDisplayList() { rebuild_display_list = true; }
};

class CPlane {
    /* levels/planes. each plane stores a tree of CSquare type and its z coordinate */
public:
    int         z;

    CSquare     *squares;
    CPlane      *next;

    CPlane();
    ~CPlane();
    CPlane(CRoom *room);
};



#endif
