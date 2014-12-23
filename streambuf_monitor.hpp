#include <map>
#include <streambuf>

#include "interlocked_traits.hpp"

namespace cxx_utils
{
    namespace io
    {
        class streambuf_callback
        {
        public:
            enum streambuf_cb_result
            {
                CB_NONE,
                CB_DROP_STREAM
            };

            enum streambuf_cb_status
            {
                CB_READ_OK,
                CB_WRITE_OK,
                CB_OPENED,
                CB_CLOSED
            };
            
            streambuf_callback(){}
            virtual ~streambuf_callback(){}

            virtual streambuf_cb_result callback(
                streambuf_cb_status eStatus, std::streambuf &rStreamBuf)=0;
        };

        template <class interlocked_trait = cxx_utils::null_interlocked_trait>
        class streambuf_monitor
        {
            std::map<std::streambuf*,streambuf_callback&> m_cCallbacks;
            interlocked_trait :: locked_exchange_type m_eLock;
        public:
            streambuf_monitor() {}
            ~streambuf_monitor() {}

            bool push(std::streambuf *pBuffer, streambuf_callback &rCB)
            {
                interlocked_trait::locked_exchange e(m_eLock);
                m_cCallbacks[pBuffer] = rCB;
            }

            bool pop(std::streambuf *pBuffer)
            {
                interlocked_trait::locked_exchange e(m_eLock);
                m_cCallbacks.erase(pBuffer);
            }

            void operator()
            {
                
            }
        };
    }
}
