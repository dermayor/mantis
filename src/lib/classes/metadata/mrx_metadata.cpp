//
// Created by Jon Kler on 5/6/25.
//

#include "mantis/lib/classes/metadata/mrx_metadata.h"

#ifdef M_UHD

uhd::rx_metadata_t mantis::go::mrx_metadata::convert_to_uhd_md_rx() {
    uhd::rx_metadata_t md_rx;
    md_rx.start_of_burst = this->start_of_burst;
    md_rx.has_time_spec = this->has_time_spec;
    md_rx.time_spec = {this->time_spec.get_full_secs(), this->time_spec.get_frac_secs()};
    md_rx.end_of_burst = this->end_of_burst;
    md_rx.eov_positions = this->eov_positions;
    md_rx.eov_positions_size = this->eov_positions_size;
    md_rx.eov_positions_count = this->eov_positions_count;
    md_rx.more_fragments = this->more_fragments;
    md_rx.out_of_sequence = this->out_of_sequence;
    md_rx.fragment_offset = this->fragment_offset;
    md_rx.error_code = static_cast<uhd::rx_metadata_t::error_code_t>(this->code); // This is fine since the mantis error codes are mapped specifically to uhd, you may have to do some manipulation for other drivers.

    return md_rx;
}

void mantis::go::mrx_metadata::from_uhd_md_rx(mantis::go::mrx_metadata& params, uhd::rx_metadata_t md) {
    params.start_of_burst = md.start_of_burst;
    params.has_time_spec = md.has_time_spec;
    params.time_spec = {md.time_spec.get_full_secs(),md.time_spec.get_frac_secs()};
    params.end_of_burst = md.end_of_burst;
    params.eov_positions = md.eov_positions;
    params.eov_positions_size = md.eov_positions_size;
    params.eov_positions_count = md.eov_positions_count;
    params.more_fragments = md.more_fragments;
    params.out_of_sequence = md.out_of_sequence;
    params.fragment_offset = md.fragment_offset;
    params.code = static_cast<metadata_errors::mrx_metadata_error_codes>(md.error_code); // This is fine since the mantis error codes are mapped specifically to uhd, you may have to do some manipulation for other drivers.

}

#endif

void mantis::go::mrx_metadata::set_time_spec(time_tag _time_spec) {
    this->time_spec = _time_spec;
    this->has_time_spec = true;
}
