#ifndef NAMEDPROCESSTABLE_HPP
#define NAMEDPROCESSTABLE_HPP
#include <map>
#include "Runtime/Sync.hpp"
#include "lua.hpp"

namespace GLR {
	class ServerExist : public Galaxy::GalaxyRT::CException{
	public:
		GALA_DEFINE_EXCEPTION(ServerExist, Galaxy::GalaxyRT::CException)
	};

    class NotRegistered : public Galaxy::GalaxyRT::CException{
    public:
		GALA_DEFINE_EXCEPTION(NotRegistered, Galaxy::GalaxyRT::CException)
    };



	class NamedProcessTable : public Galaxy::GalaxyRT::CNonCopyAble
	{
		typedef uint32_t  IDTYPE;
        enum{
            INVLID_ID = -1
        };
	private:
		NamedProcessTable();
	public:
		~NamedProcessTable();
		IDTYPE Get(const std::string &name);
		void   Set(const std::string &name, IDTYPE id);
		void   Del(const std::string &name);
	public:
		static int Register(lua_State *l);
		static int Registered(lua_State *l);
		static int UnRegister(lua_State *l);
	private:
		void _Registered(lua_State *l);
        void _UnRegistered(lua_State *l);
	public:
        static NamedProcessTable *GetInstance();
        static void CreateOpTable(lua_State *l);
        static void Clean(lua_State *l);
	private:
		static NamedProcessTable *_Instance;
        static Galaxy::GalaxyRT::CPthreadMutex _InstanceLock;
	private:
		typedef std::map<std::string, IDTYPE> ID_MAP_TYPE;
		ID_MAP_TYPE   _Map;
		Galaxy::GalaxyRT::CRWLock       _Lock;
		Galaxy::GalaxyRT::CRWLockAdapter _RDLock;
		Galaxy::GalaxyRT::CRWLockAdapter _WRLock;
	};

}
#endif // NAMEDPROCESSTABLE_HPP
