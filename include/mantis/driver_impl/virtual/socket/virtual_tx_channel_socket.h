#pragma once

#include <fstream>
#include <mantis/lib/driver_specific/interfaces/i_tx_channel.h>
#include <mantis/driver_impl/virtual/interface_impl/virtual_channel.h>
#include <mantis/driver_impl/virtual/socket/net_utils/msocket.h>

namespace mantis::driver_impl::virtual_i {
    /**
    * @brief virtual tx from socket class
    */
    class virtual_tx_channel_socket : public virtual_channel<interfaces::i_tx_channel> {
       public:
        virtual_tx_channel_socket(const params::msdr_params& _params, const params::mchannel_params& _channel_params);
        
        ~virtual_tx_channel_socket() override = default;

        void send(const char *data, size_t sample_size, size_t num_samples,
                  go::mtx_metadata &tx_metadata) override;

        //! just returns success
        mantis::errors::error_code recv_async_message(go::async_mtx_metadata& message, double timeout) override;


    private:
        net::msocket sock;
    };
}  // namespace mantis::driver_impl::virtual_i