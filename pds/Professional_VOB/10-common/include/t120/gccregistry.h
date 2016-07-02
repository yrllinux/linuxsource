#ifndef _GCC_REGISTRY_H
#define _GCC_REGISTRY_H

//#include "PlatformSystem.h"

#include "mcstypes.h"
#include "gcctypes.h"

namespace Gcc
{
	
	typedef struct RegistryEntryOwner 
	{
		int m_entityId;
		TUserId m_nodeId;
	} RegistryEntryOwner;
	
	enum RegistryModificationRights
	{
		RmrOwner,
		RmrSession,
		RmrPublic
	};
	
	class RegistryItem
	{
	public:
		enum Type 
		{
			Channel,
			Token,
			Parameter,
			Vacant
		};
		
		RegistryItem() : m_hasOwner(false), m_modificationRights(RmrPublic) {}
		
		TChannelId GetChannelId() const 
		{ 
			return m_data.m_channel; 
		}

		void SetChannelId(TChannelId c) 
		{ 
			m_data.m_channel = c; 
		}

		TTokenId GetTokenId() const 
		{ 
			return m_data.m_token;
		}

		void SetTokenId(TTokenId t) 
		{ 
			m_data.m_token = t;
		}

		const PString &GetParameter() const 
		{ 
			return m_parameter; 
		}

		void SetParameter(const PString &p) 
		{ 
			m_parameter = p;
		}
		
		void SetHasOwner(bool b) 
		{ 
			m_hasOwner = b; 
		}

		bool HasOwner() const 
		{ 
			return m_hasOwner;
		}

		void SetHasModificationRights(bool b) 
		{
			m_hasModificationRights = b; 
		}

		bool HasModificationRights() const 
		{ 
			return m_hasModificationRights;
		}

		void SetOwner(TUserId nodeId, int entid) 
		{ 
			m_hasOwner = true; 
			m_owner.m_nodeId = nodeId;
			m_owner.m_entityId = entid; 
		}

		void GetOwner(RegistryEntryOwner &owner) const 
		{ 
			owner = m_owner; 
		}

		void SetModificationRights(RegistryModificationRights rights) 
		{
			m_hasModificationRights = true; 
			m_modificationRights = rights; 
		}
		
		RegistryModificationRights GetModificationRights() const 
		{
			return m_modificationRights;
		}
		
		Type GetType() const 
		{ 
			return m_type; 
		}

		void SetType(Type t) 
		{ 
			m_type = t; 
		}
		
	private:
		Type m_type;
		union 
		{
			TChannelId m_channel;
			TTokenId m_token;
		} m_data;
		PString m_parameter;
		bool m_hasOwner;
		RegistryEntryOwner m_owner;
		bool m_hasModificationRights;
		RegistryModificationRights m_modificationRights;
	};
	
	class RegistryResponse
	{
	public:
		enum Primitive
		{
			RegisterChannel,
			AssignToken,
			SetParameter,
			RetrieveEntry,
			DeleteEntry,
			MonitorEntry
		};
		
		enum Result
		{
			Successful,
			BelongsToOther,
			TooManyEntries,
			InconsistentType,
			EntryNotFound,
			EntryAlreadyExists,
			InvalidRequester
		};
		
		Primitive GetPrimitive() const
		{ 
			return m_primitive; 
		}

		Result GetResult() const 
		{ 
			return m_result; }
		
		int GetEntityId() const 
		{
			return m_entityId; 
		}

		void SetEntityId(int e) 
		{
			m_entityId = e; 
		}
		
		const SessionKey &GetSessionKey() const 
		{ 
			return m_sessionKey; 
		}

		SessionKey &GetSessionKey() 
		{ 
			return m_sessionKey; 
		}

		const PBYTEArray &GetRegistryKey() const 
		{
			return m_registryKey; 
		}
		
		const RegistryItem &GetItem() const 
		{ 
			return m_item; 
		}

		RegistryItem &GetItem()
		{ 
			return m_item; 
		}
		
		void SetRegistryKey(const PBYTEArray &n)
		{ 
			m_registryKey = n; 
		}
		
		void SetPrimitive(Primitive p) 
		{ 
			m_primitive = p; 
		}

		void SetResult(Result r) 
		{
			m_result = r;
		}
	private:
		int m_entityId;
		Primitive m_primitive;
		Result m_result;
		SessionKey m_sessionKey;
		PBYTEArray m_registryKey;
		RegistryItem m_item;		
	};	

    class RegistryAllocateHandleResponse
    {
    public:        
        enum Result
        {
            Successful,
            NoHandlesAvailable
        };
        
        int GetEntityId() const 
        {
            return m_entityId; 
        }
        
        void SetEntityId(int e) 
        {
            m_entityId = e; 
        }
        
        int GetNumOfHandles() const 
        {
            return m_numOfHandles; 
        }
        
        void SetNumOfHandles(int e) 
        {
            m_numOfHandles = e; 
        }
        
        int GetFirstHandle() const 
        {
            return m_firstHandle; 
        }
        
        void SetFirstHandle(int e) 
        {
            m_firstHandle = e; 
        }
        
        Result GetResult() const 
        { 
            return m_result; 
        }
        
        void SetResult(Result r) 
        {
            m_result = r;
        }
    private:
        int m_entityId;
        int m_numOfHandles;
        int m_firstHandle;
        Result m_result;
    };
}

#endif

