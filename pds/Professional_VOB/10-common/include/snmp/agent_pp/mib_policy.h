/*_############################################################################
  _## 
  _##  mib_policy.h  
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



#ifndef _mib_policy_h_
#define _mib_policy_h_

#include <agent_pp/agent++.h>
#include <agent_pp/mib.h>
#include <agent_pp/snmp_target_mib.h>
#include <agent_pp/snmp_notification_mib.h>
#include <agent_pp/v3_mib.h>
#include <agent_pp/mib_complex_entry.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

struct table_size_def {
	Oidx		tableID;
	unsigned int	sizeLimit;
	table_size_def(const Oidx& o, unsigned int i): tableID(o), sizeLimit(i)
	  { }
};

/*----------------------- class MibTablePolicy -------------------------*/

/**
 * The MibTableSizePolicy class implements a policy for MibTable
 * size limits. 
 * 
 * @author Frank Fock
 * @version 3.5
 */

class AGENTPP_DECL MibTableSizePolicy: public MibTableVoter {

 public:
	/**
	 * Create an empty MibTable size policy. This will accept any
	 * table sizes for any tables.
	 */
	MibTableSizePolicy(): defaultLimit(0) { }

	/**
	 * Create a MibTable size policy using the given size restriction
	 * array. Tables with an OID not listed in the array will be
	 * restricted to the given default size.
	 *
	 * @param tableSizeRestrictions
	 *    an array of table_size_def structures. Each entry defines
	 *    the maximum number of rows that can be created for the
	 *    specified table through SNMP requests.
	 * @param size
	 *    the size of the above table.
	 * @param defaultTableSize
	 *    the maximum table size for tables not explicitly listed
	 *    in the above array. The value 0 will disable any default
	 *    restriction.
	 */
	MibTableSizePolicy(const table_size_def*, unsigned int,
			   unsigned int);

        virtual ~MibTableSizePolicy() { }

	virtual int is_transition_ok(MibTable*, MibTableRow*, const Oidx&,
				     int, int);

	/**
	 * Set a MibTable size policy.
	 *
	 * @param tableOID
	 *    the OID of a MibTable object.
	 * @param maxNumberOfRows
	 *    the upper limit for the number of rows that may exists
	 *    in the specified table. If this limit is exceeded row creation
	 *    SNMP requests will be rejected with a 'resourceUnavail'
	 *    error.
	 */   
	virtual void set_policy(const Oidx&, unsigned int);

	/**
	 * Remove a size restriction from the policy table.
	 *
	 * @param tableOID
	 *    the OID of a MibTable object.
	 */
	virtual void remove_policy(const Oidx&);

	/**
	 * Register the given MibTableSizePolicy instance wiht all
	 * tables currently known by the given Mib instance.
	 *
	 * @param mib
	 *    a pointer to a Mib instance that has already tables
	 *    registered.
	 * @param sizePolicy
	 *    a pointer to a MibTableSizePolicy instance.
	 */
	static void register_policy(MibContext*, MibTableSizePolicy*);

 protected:
	OidList<MibStaticEntry> policies;
	unsigned int		defaultLimit;
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
