#include <mantis/driver_impl/virtual/interface_impl/virtual_channel.h>
#include <mantis/driver_impl/virtual/socket/net_utils/msocket.h>
#include <mantis/lib/driver_specific/interfaces/i_rx_channel.h>

namespace mantis::driver_impl::virtual_i {
    /**
     * @brief virtual rx from socket class
     */
    class virtual_rx_channel_socket : public virtual_channel<interfaces::i_rx_channel> {
      public:
        virtual_rx_channel_socket(const params::msdr_params& _params, params::mchannel_params _channel_params);

        ~virtual_rx_channel_socket() override = default;

        /**
         * @note The socket channel does not currently support receiving less than
         */
        void receive(char* data, size_t sample_size, size_t num_samples, go::mrx_metadata& rx_metadata) override;

      private:
        net::msocket sock;
    };
} // namespace mantis::driver_impl::virtual_i
