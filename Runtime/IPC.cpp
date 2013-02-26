/* -*- C++ -*- */

// Time-stamp: <2012-04-17 09:16:51 星期二 by OCaml>


/**
 * @file IPC.cpp
 * @author qingant
 */
#include "IPC.hpp"

namespace Galaxy
{
   namespace GalaxyRT
   {

// impl of CIPC
      CIPC::~CIPC()
      {

      }

      CMsgQ::CMsgQ(const std::string &name, INT oflags):Name(name),Key(CRT_ftok(Name.c_str(),1))
      {
         if(Key == -1)
         {
			THROW_EXCEPTION_EX("failure to ftok");
         }
         MsgID = CRT_msgget(Key, oflags);
         if(MsgID == -1)
         {
			THROW_EXCEPTION_EX("failure to msgget");
         }
      }

      INT CMsgQ::Remove()
      {
         return CRT_msgctl(MsgID, IPC_RMID, NULL);
      }

      INT CMsgQ::Send(const VOID *buf, size_t len, INT flag) const
      {
         return CRT_msgsnd(MsgID, buf, len - sizeof(LONG), flag);
      }

      INT CMsgQ::SendNoWait(const VOID *buf, size_t len) const
      {
         return Send(buf, len, IPC_NOWAIT);
      }

      INT CMsgQ::Recv(VOID *buf, size_t len, LONG type, INT flag) const
      {
         return CRT_msgrcv(MsgID, buf, len-sizeof(LONG), type, flag);
      }

      INT CMsgQ::RecvNoWait(VOID *buf, size_t len, LONG type) const
      {
         return Recv(buf, len, type, IPC_NOWAIT);
      }

      CMsgQ::~CMsgQ()
      {

      }


      CPosixShmObj::CPosixShmObj(CPSTR name, INT oflag, mode_t mode)
         :CIOHandle(CRT_shm_open(name, oflag, mode)),
          m_name(name)
      {

      }
      const std::string &CPosixShmObj::GetName() const
      {
         return m_name;
      }
      void CPosixShmObj::Close()
      {

         if(CRT_unlink(m_name.c_str()) == -1)
         {
			THROW_SYSERROR();
         }
      }

      CPosixShmObj::~CPosixShmObj()
      {
         try
         {
			Close();
         }
         catch(...)
         {
			return;
         }
      }


// implemention of CSHM
      CSHM::CSHM(std::auto_ptr<CIOHandle> handle, size_t size)
         :m_handle(handle),
          m_size(size)
      {
         //INT rt = CRT_fstat(ShmFd, &ShmStat);
      }

      CSHM::~CSHM()
      {

      }

      void CSHM::Unlink()
      {
         //Detach();
         m_handle->Close();
      }

// INT CSHM::Ftruncate(off_t len)
// {
//     handle.ftruncate(len);
// }

      PVOID CSHM::Attach()
      {
         return this->Attach(PROT_READ);
      }
      PVOID CSHM::Attach(INT prot)
      {
         m_pShm = CRT_mmap(NULL,m_size, prot, MAP_SHARED, m_handle->GetFD(), 0);
         if (m_pShm == MAP_FAILED)
         {
			THROW_SYSERROR();
         }
         return m_pShm;
      }

      void CSHM::Detach()
      {
         Detach(m_size);
      }
      void CSHM::Detach(size_t len)
      {
         INT ret = CRT_munmap(m_pShm, len);
         if (ret == -1)
         {
			THROW_SYSERROR();
         }
      }



      void CSHM::Msync(size_t len, INT flag )
      {
         INT ret = CRT_msync(m_pShm, len, flag);
         if (ret == -1)
         {
			THROW_SYSERROR();
         }

      }

      void CSHM::Msync(INT flag )
      {
         Msync(m_size, flag);
      }






      CSystemVSHM::CSystemVSHM(key_t key, size_t size, INT oflag)
         :m_key(key),
          m_identifier(-1),
          m_pshm(NULL)
      {
         if ((m_identifier = CRT_shmget(key, size, oflag)) == -1)
         {
			THROW_SYSERROR();
         }
         if ((m_pshm = CRT_shmat(m_identifier, NULL, 0)) == (void*)(-1))
         {
			THROW_SYSERROR();
         }
      }

      PVOID CSystemVSHM::GetSHM() const
      {
         return m_pshm;
      }

      void  CSystemVSHM::Detach()
      {
         if ((m_pshm != NULL) && (CRT_shmdt(m_pshm) == -1))
         {
			THROW_SYSERROR();
         }
         m_pshm = NULL;
      }

      void  CSystemVSHM::Remove()
      {
         if(CRT_shmctl(m_identifier, IPC_RMID, NULL) == -1)
         {
			THROW_SYSERROR();
         }
      }

      CSystemVSHM::~CSystemVSHM()
      {
         try
         {
			Detach();
         }
         catch(...)
         {
			return;
         }
      }

   } /// namespace GalaxyRT
} ///namespace Galaxy
