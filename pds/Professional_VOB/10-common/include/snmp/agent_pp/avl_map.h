/*_############################################################################
  _## 
  _##  avl_map.h  
  _## 
  _##
  _##  AGENT++ API Version 3.5.23a
  _##  -----------------------------------------------
  _##  Copyright (C) 2000-2005 Frank Fock, Jochen Katz
  _##  
  _##  LICENSE AGREEMENT
  _##
  _##  WHEREAS,  Frank  Fock  and  Jochen  Katz  are  the  owners of valuable
  _##  intellectual  property rights relating to  the AGENT++ API and wish to
  _##  license AGENT++ subject to the  terms and conditions set forth  below;
  _##  and
  _##
  _##  WHEREAS, you ("Licensee") acknowledge  that Frank Fock and Jochen Katz
  _##  have the right  to grant licenses  to the intellectual property rights
  _##  relating to  AGENT++, and that you desire  to obtain a license  to use
  _##  AGENT++ subject to the terms and conditions set forth below;
  _##
  _##  Frank  Fock    and Jochen   Katz   grants  Licensee  a  non-exclusive,
  _##  non-transferable, royalty-free  license  to use   AGENT++ and  related
  _##  materials without  charge provided the Licensee  adheres to all of the
  _##  terms and conditions of this Agreement.
  _##
  _##  By downloading, using, or  copying  AGENT++  or any  portion  thereof,
  _##  Licensee  agrees to abide  by  the intellectual property  laws and all
  _##  other   applicable laws  of  Germany,  and  to all of   the  terms and
  _##  conditions  of this Agreement, and agrees  to take all necessary steps
  _##  to  ensure that the  terms and  conditions of  this Agreement are  not
  _##  violated  by any person  or entity under the  Licensee's control or in
  _##  the Licensee's service.
  _##
  _##  Licensee shall maintain  the  copyright and trademark  notices  on the
  _##  materials  within or otherwise  related   to AGENT++, and  not  alter,
  _##  erase, deface or overprint any such notice.
  _##
  _##  Except  as specifically   provided in  this  Agreement,   Licensee  is
  _##  expressly   prohibited  from  copying,   merging,  selling,   leasing,
  _##  assigning,  or  transferring  in  any manner,  AGENT++ or  any portion
  _##  thereof.
  _##
  _##  Licensee may copy materials   within or otherwise related   to AGENT++
  _##  that bear the author's copyright only  as required for backup purposes
  _##  or for use solely by the Licensee.
  _##
  _##  Licensee may  not distribute  in any  form  of electronic  or  printed
  _##  communication the  materials  within or  otherwise  related to AGENT++
  _##  that  bear the author's  copyright, including  but  not limited to the
  _##  source   code, documentation,  help  files, examples,  and benchmarks,
  _##  without prior written consent from the authors.  Send any requests for
  _##  limited distribution rights to fock@agentpp.com.
  _##
  _##  Licensee  hereby  grants  a  royalty-free  license  to  any  and   all 
  _##  derivatives  based  upon this software  code base,  that  may  be used
  _##  as a SNMP  agent development  environment or a  SNMP agent development 
  _##  tool.
  _##
  _##  Licensee may  modify  the sources  of AGENT++ for  the Licensee's  own
  _##  purposes.  Thus, Licensee  may  not  distribute  modified  sources  of
  _##  AGENT++ without prior written consent from the authors. 
  _##
  _##  The Licensee may distribute  binaries derived from or contained within
  _##  AGENT++ provided that:
  _##
  _##  1) The Binaries are  not integrated,  bundled,  combined, or otherwise
  _##     associated with a SNMP agent development environment or  SNMP agent
  _##     development tool; and
  _##
  _##  2) The Binaries are not a documented part of any distribution material. 
  _##
  _##
  _##  THIS  SOFTWARE  IS  PROVIDED ``AS  IS''  AND  ANY  EXPRESS OR  IMPLIED
  _##  WARRANTIES, INCLUDING, BUT NOT LIMITED  TO, THE IMPLIED WARRANTIES  OF
  _##  MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.
  _##  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  _##  INDIRECT,   INCIDENTAL,  SPECIAL, EXEMPLARY,  OR CONSEQUENTIAL DAMAGES
  _##  (INCLUDING,  BUT NOT LIMITED  TO,  PROCUREMENT OF SUBSTITUTE  GOODS OR
  _##  SERVICES; LOSS OF  USE,  DATA, OR PROFITS; OR  BUSINESS  INTERRUPTION)
  _##  HOWEVER CAUSED  AND ON ANY THEORY  OF  LIABILITY, WHETHER IN CONTRACT,
  _##  STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
  _##  IN  ANY WAY OUT OF  THE USE OF THIS  SOFTWARE,  EVEN IF ADVISED OF THE
  _##  POSSIBILITY OF SUCH DAMAGE. 
  _##
  _##
  _##  Stuttgart, Germany, Sat Jun 18 13:06:20 CEST 2005 
  _##  
  _##########################################################################*/
// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#ifndef _OidxPtrEntryPtrAVLMap_h
#ifdef __GNUG__
#pragma interface
#endif
#define _OidxPtrEntryPtrAVLMap_h 1

#include <agent_pp/map.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


struct OidxPtrEntryPtrAVLNode
{
  OidxPtrEntryPtrAVLNode*      lt;
  OidxPtrEntryPtrAVLNode*      rt;
  OidxPtr                 item;
  EntryPtr                 cont;
  char                stat;
                      OidxPtrEntryPtrAVLNode(OidxPtr  h, EntryPtr  c, 
                                    OidxPtrEntryPtrAVLNode* l=0, OidxPtrEntryPtrAVLNode* r=0);
                      ~OidxPtrEntryPtrAVLNode();
};

inline OidxPtrEntryPtrAVLNode::OidxPtrEntryPtrAVLNode(OidxPtr  h, EntryPtr  c, 
                                    OidxPtrEntryPtrAVLNode* l, OidxPtrEntryPtrAVLNode* r)
     :lt(l), rt(r), item(h), cont(c), stat(0) {}

inline OidxPtrEntryPtrAVLNode::~OidxPtrEntryPtrAVLNode() {}

typedef OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLNodePtr;


class AGENTPP_DECL OidxPtrEntryPtrAVLMap : public OidxPtrEntryPtrMap
{
protected:
  OidxPtrEntryPtrAVLNode*   root;

  OidxPtrEntryPtrAVLNode*   leftmost() const;
  OidxPtrEntryPtrAVLNode*   rightmost() const;
  OidxPtrEntryPtrAVLNode*   pred(OidxPtrEntryPtrAVLNode* t) const;
  OidxPtrEntryPtrAVLNode*   succ(OidxPtrEntryPtrAVLNode* t) const;
  void            _kill(OidxPtrEntryPtrAVLNode* t);
  void            _add(OidxPtrEntryPtrAVLNode*& t);
  void            _del(OidxPtrEntryPtrAVLNode* p, OidxPtrEntryPtrAVLNode*& t);

public:
                OidxPtrEntryPtrAVLMap(EntryPtr  dflt);
                OidxPtrEntryPtrAVLMap(OidxPtrEntryPtrAVLMap& a);
  inline		~OidxPtrEntryPtrAVLMap();

  EntryPtr&          operator [] (OidxPtr  key);

  void			del(OidxPtr  key);

  inline Pix            first() const;
  inline void           next(Pix& i) const;
  inline OidxPtr&       key(Pix i) const;
  inline EntryPtr&   contents(Pix i);

  Pix			seek(OidxPtr  key) const;
  Pix			seek_inexact(OidxPtr  key) const;
  inline int		contains(OidxPtr  key) const;

  inline void		clear(); 

  Pix			last() const;
  void			prev(Pix& i) const;

  int			OK();
};

inline OidxPtrEntryPtrAVLMap::~OidxPtrEntryPtrAVLMap()
{
  _kill(root);
}

inline OidxPtrEntryPtrAVLMap::OidxPtrEntryPtrAVLMap(EntryPtr  dflt) :OidxPtrEntryPtrMap(dflt)
{
  root = 0;
}

inline Pix OidxPtrEntryPtrAVLMap::first() const
{
  return Pix(leftmost());
}

inline Pix OidxPtrEntryPtrAVLMap::last() const
{
  return Pix(rightmost());
}

inline void OidxPtrEntryPtrAVLMap::next(Pix& i) const
{
  if (i != 0) i = Pix(succ((OidxPtrEntryPtrAVLNode*)i));
}

inline void OidxPtrEntryPtrAVLMap::prev(Pix& i) const
{
  if (i != 0) i = Pix(pred((OidxPtrEntryPtrAVLNode*)i));
}

inline OidxPtr& OidxPtrEntryPtrAVLMap::key(Pix i) const
{
  if (i == 0) error("null Pix");
  return ((OidxPtrEntryPtrAVLNode*)i)->item;
}

inline EntryPtr& OidxPtrEntryPtrAVLMap::contents(Pix i)
{
  if (i == 0) error("null Pix");
  return ((OidxPtrEntryPtrAVLNode*)i)->cont;
}

inline void OidxPtrEntryPtrAVLMap::clear()
{
  _kill(root);
  count = 0;
  root = 0;
}

inline int OidxPtrEntryPtrAVLMap::contains(OidxPtr  key) const
{
  return seek(key) != 0;
}
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
