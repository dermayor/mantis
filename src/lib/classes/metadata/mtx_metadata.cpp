//
// Created by Jon Kler on 4/17/25.
//

#include "mantis/lib/classes/metadata/mtx_metadata.h"

#ifdef M_UHD

uhd::tx_metadata_t mantis::go::mtx_metadata::convert_to_uhd_md_tx() {
    uhd::tx_metadata_t md_tx;
    md_tx.start_of_burst = this->start_of_burst;
    md_tx.has_time_spec = this->has_time_spec;
    md_tx.time_spec = {this->time_spec.get_full_secs(), this->time_spec.get_frac_secs()};
    md_tx.end_of_burst = this->end_of_burst;
    md_tx.eov_positions = this->eov_positions;
    md_tx.eov_positions_size = this->eov_positions_size;

    return md_tx;
}


void mantis::go::mtx_metadata::from_uhd_md_tx(mantis::go::mtx_metadata &params, uhd::tx_metadata_t md) {
    params.has_time_spec = md.has_time_spec;
    params.start_of_burst = md.start_of_burst;
    params.time_spec = {md.time_spec.get_full_secs(), md.time_spec.get_frac_secs()};
    params.end_of_burst = md.end_of_burst;
    params.eov_positions = md.eov_positions;
    params.eov_positions_size = md.eov_positions_size;
}

#endif

void mantis::go::mtx_metadata::set_time_spec(time_tag _time_spec) {
    this->time_spec = _time_spec;
    this->has_time_spec = true;
}
