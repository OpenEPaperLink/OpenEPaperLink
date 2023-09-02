/* mbed Microcontroller Library
 * Copyright (c) 2018-2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include <stdint.h>
 
#include "events/EventQueue.h"
 
#include "nfc/controllers/PN512Driver.h"
#include "nfc/controllers/PN512SPITransportDriver.h"
#include "nfc/NFCRemoteInitiator.h"
#include "nfc/NFCController.h"
 
#include "nfc/ndef/MessageBuilder.h"
#include "nfc/ndef/common/util.h"
 
#include "SmartPoster.h"
 
using events::EventQueue;
 
using mbed::Span;
using mbed::nfc::NFCRemoteInitiator;
using mbed::nfc::NFCController;
using mbed::nfc::nfc_rf_protocols_bitmask_t;
using mbed::nfc::ndef::MessageBuilder;
using mbed::nfc::ndef::common::Text;
using mbed::nfc::ndef::common::URI;
using mbed::nfc::ndef::common::span_from_cstr;
 
/**
 * Manage the NFC discovery process and the local device operating in target mode.
 *
 * When a remote initiator has been discovered, it connects to it then reply
 * to its ndef message request with a smart poster message that contains:
 *   - A URI: https://www.mbed.com
 *   - A title: mbed website
 *   - An action: EXECUTE which opens the browser of the peer with the URI
 *   transmitted.
 */
class NFCProcess : NFCRemoteInitiator::Delegate, NFCController::Delegate {
public:
    /**
     * Construct a new NFCProcess objects.
     *
     * This function construct the NFC controller and wires it with the PN512
     * driver.
     *
     * @param queue The event queue that will be used by the NFCController.
     */
    NFCProcess(events::EventQueue &queue) :
        _pn512_transport(D11, D12, D13, D10, A1, A0),
        _pn512_driver(&_pn512_transport),
        _queue(queue),
        _ndef_buffer(),
        _nfc_controller(&_pn512_driver, &queue, _ndef_buffer)
    { }
 
    /**
     * Initialise and configure the NFC controller.
     *
     * @return NFC_OK in case of success or a meaningful error code in case of
     * failure.
     */
    nfc_err_t init()
    {
        nfc_err_t err = _nfc_controller.initialize();
        if (err) {
            return err;
        }
 
        // register callbacks
        _nfc_controller.set_delegate(this);
 
        nfc_rf_protocols_bitmask_t protocols = { 0 };
        protocols.target_iso_dep = 1;
        return _nfc_controller.configure_rf_protocols(protocols);
    }
 
    /**
     * Start the discovery of peers.
     *
     * @return NFC_OK in case of success or a meaningful error code in case of
     * failure.
     */
    nfc_err_t start_discovery()
    {
        return _nfc_controller.start_discovery();
    }
 
private:
    /* ------------------------------------------------------------------------
     * Implementation of NFCRemoteInitiator::Delegate
     */
    virtual void on_connected() { }
 
    virtual void on_disconnected()
    {
        // reset the state of the remote initiator
        _nfc_remote_initiator->set_delegate(NULL);
        _nfc_remote_initiator.reset();
 
        // restart peer discovery
        _nfc_controller.start_discovery();
    }
 
    virtual void parse_ndef_message(const Span<const uint8_t> &buffer) { }
 
    virtual size_t build_ndef_message(const Span<uint8_t> &buffer)
    {
        // build the smart poster object we want to send
        SmartPoster smart_poster(
            URI(URI::HTTPS_WWW, span_from_cstr("mbed.com"))
        );
        smart_poster.set_title(
            Text(Text::UTF8, span_from_cstr("en-US"), span_from_cstr("mbed website"))
        );
        smart_poster.set_action(SmartPoster::EXECUTE);
 
        // serialize the smart poster into an ndef message operating on the
        // buffer in input.
        MessageBuilder builder(buffer);
        smart_poster.append_record(builder, /* last ? */ true);
 
        return builder.get_message().size();
    }
 
    /* ------------------------------------------------------------------------
     * Implementation of NFCController::Delegate
     */
    virtual void on_discovery_terminated(nfc_discovery_terminated_reason_t reason)
    {
        if(reason != nfc_discovery_terminated_completed) {
            _nfc_controller.start_discovery();
        }
    }
 
    virtual void on_nfc_initiator_discovered(const SharedPtr<NFCRemoteInitiator> &nfc_initiator)
    {
        // setup the local remote initiator
        _nfc_remote_initiator = nfc_initiator;
        _nfc_remote_initiator->set_delegate(this);
        _nfc_remote_initiator->connect();
    }
 
    mbed::nfc::PN512SPITransportDriver _pn512_transport;
    mbed::nfc::PN512Driver _pn512_driver;
    EventQueue& _queue;
    uint8_t _ndef_buffer[1024];
    NFCController _nfc_controller;
    SharedPtr<NFCRemoteInitiator> _nfc_remote_initiator;
};
 
int main()
{
    events::EventQueue queue;
    NFCProcess nfc_process(queue);
 
    nfc_err_t ret = nfc_process.init();
    printf("Initialize: ret = %u\r\n", ret);
 
    ret = nfc_process.start_discovery();
    printf("Start Discovery: ret = %u\r\n", ret);
 
    queue.dispatch_forever();
 
    return 0;
}