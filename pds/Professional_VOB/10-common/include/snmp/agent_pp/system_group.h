/*_############################################################################
  _## 
  _##  system_group.h  
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

#ifndef system_group_h_
#define system_group_h_

#ifndef WIN32
#include <sys/time.h>
#endif

#include <string.h>
#include <agent_pp/mib.h>
#include <agent_pp/snmp_textual_conventions.h>

#define oidSysGroup		"1.3.6.1.2.1.1"
#define oidSysDescr		"1.3.6.1.2.1.1.1.0"
#define oidSysObjectID		"1.3.6.1.2.1.1.2.0"
#define oidSysUpTime		"1.3.6.1.2.1.1.3.0"
#define oidSysContact		"1.3.6.1.2.1.1.4.0"
#define oidSysName		"1.3.6.1.2.1.1.5.0"
#define oidSysLocation		"1.3.6.1.2.1.1.6.0"
#define oidSysServices		"1.3.6.1.2.1.1.7.0"

#define confSysName		"sysName.leaf"
#define confSysContact		"sysContact.leaf"
#define confSysLocation		"sysLocation.leaf"
#define confSysORTable		"sysORTable.table"

#define oidSysORLastChange               "1.3.6.1.2.1.1.8.0"
#define oidSysORTable                    "1.3.6.1.2.1.1.9"
#define oidSysOREntry                    "1.3.6.1.2.1.1.9.1"
#define oidSysORIndex                    "1.3.6.1.2.1.1.9.1.1"
#define colSysORIndex                    "1"
#define oidSysORID                       "1.3.6.1.2.1.1.9.1.2"
#define colSysORID                       "2"
#define oidSysORDescr                    "1.3.6.1.2.1.1.9.1.3"
#define colSysORDescr                    "3"
#define oidSysORUpTime                   "1.3.6.1.2.1.1.9.1.4"
#define colSysORUpTime                   "4"

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/**********************************************************************
 *  
 *  class sysUpTime
 * 
 **********************************************************************/

class AGENTPP_DECL sysUpTime: public MibLeaf {

public:
	sysUpTime();

	void		get_request(Request*, int);

	static u_int	get();
	static time_t	get_currentTime();	
	
	virtual boolean is_volatile() { return TRUE; }
	static u_int	start;
};

/**********************************************************************
 *  
 *  class sysDescr
 * 
 **********************************************************************/

class AGENTPP_DECL sysDescr: public SnmpDisplayString {

public:
	sysDescr(const char*);
};

/**********************************************************************
 *  
 *  class sysObjectID
 * 
 **********************************************************************/

class AGENTPP_DECL sysObjectID: public MibLeaf {

public:
	sysObjectID(const Oidx&);
};

/**********************************************************************
 *  
 *  class sysServices
 * 
 **********************************************************************/

class AGENTPP_DECL sysServices: public MibLeaf {

public:
	sysServices(const int);
};


/**
 *  sysORLastChange
 *
"The value of sysUpTime at the time of the most recent
 change in state or value of any instance of sysORID."
 */


class AGENTPP_DECL sysORLastChange: public MibLeaf {

public:
	sysORLastChange();
	virtual ~sysORLastChange() {};

	static sysORLastChange* instance;

	virtual boolean is_volatile() { return TRUE; }
	void       	update();
};

/**
 *  sysOREntry
 *
"An entry (conceptual row) in the sysORTable."
 */


class AGENTPP_DECL sysOREntry: public TimeStampTable {

public:
	sysOREntry(TimeStamp*);
	virtual ~sysOREntry();

	static sysOREntry* instance;

	virtual void       	set_row(MibTableRow* r, 
					const Oidx&, const NS_SNMP OctetStr&, int);
	/**
	 * Check whether the receiver contains a row with a given sysORID.
	 *
	 * @param sysORID
	 *    an object identifier.
	 * @return
	 *    a pointer to the found MibTableRow instance, or 0 if such
	 *    an entry does not exist.
	 */
	virtual MibTableRow*    find(const Oidx&);

	virtual boolean is_volatile() { return TRUE; }
};




/**********************************************************************
 *  
 *  class sysGroup
 * 
 **********************************************************************/

class AGENTPP_DECL sysGroup: public MibGroup {

public:
	// must values for the system group are:
	// system descriptor, system object ID, 
	// and the services of the system

	sysGroup(const char*, const Oidx&, const int);
 private:
	char* 	path;
};
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
