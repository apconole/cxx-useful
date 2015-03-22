#include <map>
#include <streambuf>

#include "interlocked_traits.hpp"

namespace cxx_utils
{
    namespace io
    {
        /**
         * @brief A streambuf_callback object is a functional object used by the monitor
         * to inform other entities that a particular event or condition on a streambuf
         * has occured.
         *
         * The callback indicates read-ready, closure, 'open', and (possibly) streambuf
         * which has write space for non-blocking operation.
         * The intention with this class is to inherit and create a subclass which
         * overrides the callback() function.
         */
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

            /**
             * @brief Used to indicate a streambuf condition is met.
             *
             * @param eStatus The specific condition being indicated. Even if the event is CB_CLOSED, the monitor will not call delete, or release any associated memory.
             * @param pStreambuf The specific streambuf for which the condition is true
             * @return CB_NONE indicates that the monitor should keep the streambuf for additional monitoring. CB_DROP_STREAM will remove the streambuf from the processing map.
             */
            virtual streambuf_cb_result callback(
                streambuf_cb_status eStatus, std::streambuf *pStreamBuf)=0;
        };

        /**
         * A streambuf_monitor is a way of wrapping up event based IO using the std::streambuf
         * interface, and wrapping a lightweight logic around it.
         */
        template <class interlocked_trait = cxx_utils::null_interlocked_trait>
        class streambuf_monitor
        {
            std::map<std::streambuf*,streambuf_callback&> m_cCallbacks;
            std::list<std::streambuf*> m_cToDrop;
            interlocked_trait :: locked_exchange_type m_eLock;

            static void check_buffer
            (std::map<std::streambuf*,streambuf_callback&>::value_type t, std::list<std::streambuf*> &rList)
            {
                std::streambuf *pStream = t.first;
                std::streamsize nBytes = pStream->in_avail();
                streambuf_callback::streambuf_cb_result result =
                    streambuf_callback::CB_NONE;
                if( nBytes == -1 )
                {
                    result = (t.second).callback
                        (streambuf_callback::CB_CLOSED, pStream);
                } else if ( nBytes > 0 )
                {
                    result = (t.second).callback
                        (streambuf_callback::CB_READ_OK, pStream);
                }

                if( result == streambuf_callback::CB_DROP_STREAM )
                {
                    rList.push_back(pStream);
                }

            }

            struct buffer_checker
            {
                streambuf_monitor *pMon;
                void operator()(std::map<std::streambuf*,streambuf_callback&>::value_type t)
                {
                    streambuf_monitor::check_buffer(t, pMon->m_cToDrop);
                }
            };
            
        public:
            streambuf_monitor() {}
            ~streambuf_monitor() {}

            /**
             * Adds a streambuf to be monitored, with a specific callback instance.
             * Additionally, if bCallOpen is true, the streambuf callback will be told that the streambuf has been opened.
             */
            bool push(std::streambuf *pBuffer, streambuf_callback &rCB,
                      bool bCallOpen)
            {
                interlocked_trait::locked_exchange e(m_eLock);
                if ( bCallOpen )
                {
                    streambuf_callback::streambuf_cb_result result =
                        rCB.callback(streambuf_callback::CB_OPENED,
                                     pBuffer);
                    if( result == streambuf_callback::CB_DROP_STREAM )
                        return;
                }
                m_cCallbacks[pBuffer] = rCB;
            }

            /**
             * Removes a streambuf from the monitor.
             */
            bool pop(std::streambuf *pBuffer)
            {
                interlocked_trait::locked_exchange e(m_eLock);
                if(m_cCallbacks.erase(pBuffer) != m_cCallbacks.end()) return true;
                return false;
            }
            
            void operator()
            {
                interlocked_trait::locked_exchange e(m_eLock);
                buffer_checker bc;
                bc.pMon = this;
                for_each( m_cCallbacks.begin(), m_cCallbacks.end(),
                          bc );
                for(std::list<std::streambuf*>::iterator iDrop = m_cToDrop.begin();
                    iDrop != m_cToDrop.end(); ++iDrop)
                {
                    m_cCallbacks.erase(*iDrop);
                }

                m_cToDrop.clear();
            }
        };
    }
}
