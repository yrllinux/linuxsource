/*_############################################################################
  _## 
  _##  vacm_class.h  
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


#ifndef vacm_class_h_
#define vacm_class_h_

#include <agent_pp/snmp_pp_ext.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

class SnmpUnknownContexts;
class SnmpUnavailableContexts;
class VacmContextTable;
class VacmSecurityToGroupTable;
class VacmAccessTable;
class VacmViewTreeFamilyTable;
class Mib;
class Oidx;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef boolean
#define boolean int
#endif

#define VACM_accessAllowed          0
#define VACM_notInView              1
#define VACM_noSuchView             2
#define VACM_noSuchContext          3
#define VACM_noGroupName            4
#define VACM_noAccessEntry          5
#define VACM_otherError             6

#define VACM_viewFound              7

static char * vacmErrs[] = {
  (char*)"accessAllowed",  // 0
  (char*)"notInView",      // 1
  (char*)"noSuchView ",    // 2
  (char*)"noSuchContext",  // 3
  (char*)"noGroupName",    // 4
  (char*)"noAccessEntry",  // 5
  (char*)"otherError"      // 6
};


#define mibView_read     1
#define mibView_write    2
#define mibView_notify   3

#define storageType_other              1
#define storageType_volatile           2
#define storageType_nonVolatile        3
#define storageType_permanent          4
#define storageType_readOnly           5

#define match_exact  1
#define match_prefix 2

#define view_included   1
#define view_excluded   2

typedef struct {
  VacmContextTable*         contextTable;
  VacmSecurityToGroupTable* securityToGroupTable;
  VacmAccessTable*          accessTable;
  VacmViewTreeFamilyTable*  viewTreeFamilyTable;
  SnmpUnknownContexts*      snmpUnknownContexts;
  SnmpUnavailableContexts*  snmpUnavailableContexts;
} VacmClassPointers;


class AGENTPP_DECL Vacm
{
 public:
  Vacm();
  Vacm(Mib& );
  virtual ~Vacm();
  virtual int  	isAccessAllowed(int model, NS_SNMP OctetStr name, int level, 
				int viewType,
				NS_SNMP OctetStr context, Oidx o);
  virtual int   isAccessAllowed(NS_SNMP OctetStr viewName, Oidx o);
  int           getViewName(int model, NS_SNMP OctetStr name, int level,
			    int viewType, NS_SNMP OctetStr context,
			    NS_SNMP OctetStr &viewName);
  void		incUnknownContexts(); 
  NS_SNMP SnmpInt32     getUnknownContexts();
  static char*	getErrorMsg(int nr) { return vacmErrs[nr];};

  boolean	addNewContext(const NS_SNMP OctetStr &newContext);
  void          deleteContext(const NS_SNMP OctetStr &context);
  boolean	addNewGroup(int securityModel, const NS_SNMP OctetStr& securityName, 
			    const NS_SNMP OctetStr& groupName, int storageType);
  void deleteGroup(int securityModel, const NS_SNMP OctetStr& securityName);
  boolean	addNewAccessEntry(const NS_SNMP OctetStr& groupName, 
				  const NS_SNMP OctetStr& prefix, 
				  int securityModel, int securityLevel,
				  int match, const NS_SNMP OctetStr& readView, 
				  const NS_SNMP OctetStr& writeView,
				  const NS_SNMP OctetStr& notifyView, int storageType);
  void deleteAccessEntry(const NS_SNMP OctetStr& groupName,
			 const NS_SNMP OctetStr& prefix, 
                         int securityModel, int securityLevel);
  boolean	addNewView(const NS_SNMP OctetStr& viewName, const Oidx& subtree, 
			   const NS_SNMP OctetStr& mask, int type, int storageType);
  void deleteView(const NS_SNMP OctetStr& viewName, const Oidx& subtree);

  /**
   * Get the pointers to the VACM SNMP tables used by the VACM.
   *
   * @return
   *    a VacmClassPointers instance containing the pointers of the
   *    SNMP tables used by the VACM.
   */
  VacmClassPointers get_vacm_tables() { return vcp; }

 protected:
  VacmClassPointers vcp;
};

#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
