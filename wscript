# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('bundle-protocol', [])
    module.source = [
     	'model/bp-administrative-record.cc',
		'model/bp-bundle.cc',
		'model/bp-bundle-endpoint-id.cc',
		'model/bp-bundle-protocol-agent.cc',
		'model/bp-bundle-router.cc',
		'model/bp-bundle-status-report.cc',
		'model/bp-contact.cc',
		'model/bp-contact-window-information.cc',
		'model/bp-convergence-layer-agent.cc',
		'model/bp-convergence-layer-header.cc',
		'model/bp-creation-timestamp.cc',
		'model/bp-custody-signal.cc',
		'model/bp-data-gatherer.cc',
		'model/bp-dictionary.cc',
		'model/bp-direct-delivery-router.cc',
		'model/bp-forwarding-log.cc',
		'model/bp-global-bundle-identifier.cc',
		'model/bp-header.cc',
		'model/bp-known-delivered-messages.cc',
		'model/bp-link.cc',
		'model/bp-link-manager.cc',
		'model/bp-neighbourhood-detection-agent.cc',
		#'model/bp-neighbourhood-detection-agent-cw.cc',
		'model/bp-orwar-contact.cc',
		#'model/bp-orwar-link-manager.cc',
		#'model/bp-orwar-router-changed-order.cc',
		'model/bp-registration.cc',
		'model/bp-registration-endpoint.cc',
		'model/bp-registration-factory.cc',
		'model/bp-registration-manager.cc',
		'model/bp-sdnv.cc',
		'model/bp-rt-epidemic.cc',
		'model/bp-rt-prophet.cc',
		'model/bp-rt-prophet-hello.cc',
		'model/bp-neigh-header.cc',
		#'model/bp-hello-header.cc',
		'model/bp-rt-trend-of-delivery.cc',
		'model/vector2d.cc',
		'model/bp-rt-trend-of-delivery-neigh-hello.cc',
		'model/trend-of-delivery.xfs.cpp',
		'model/bp-rt-sprayandwait.cc',
		'model/xfuzzy.cpp',,
		'helper/bundle-protocol-helper.cc',
		#'helper/one-mobility-helper.cc',
		'helper/one-traffic-helper.cc',
		'model/ieee754.cc'																					
        ]

#    module_test = bld.create_ns3_module_test_library('bundle-protocol')
#    module_test.source = [
#        'test/bundle-protocol-test-suite.cc',
#        ]

    headers = bld.new_task_gen(features=['ns3header'])
    headers.module = 'bundle-protocol'
    headers.source = [
		'model/bp-administrative-record.h',
		'model/bp-bundle-endpoint-id.h',
		'model/bp-bundle.h',
		'model/bp-bundle-protocol-agent.h',
		'model/bp-bundle-router.h',
		'model/bp-bundle-status-report.h',
		'model/bp-contact.h',
		'model/bp-contact-window-information.h',
		'model/bp-convergence-layer-agent.h',
		'model/bp-convergence-layer-header.h',
		'model/bp-creation-timestamp.h',
		'model/bp-custody-signal.h',
		'model/bp-data-gatherer.h',
		'model/bp-dictionary.h',
		'model/bp-direct-delivery-router.h',
		'model/bp-forwarding-log.h',
		'model/bp-global-bundle-identifier.h',
		'model/bp-header.h',
		'model/bp-known-delivered-messages.h',
		'model/bp-link.h',
		'model/bp-link-manager.h',
		#'model/bp-neighbourhood-detection-agent-cw.h',
		'model/bp-neighbourhood-detection-agent.h',
		#'model/bp-orwar-contact.h',
		#'model/bp-orwar-link-manager.h',
		#'model/bp-orwar-router-changed-order.h',
		'model/bp-registration-endpoint.h',
		'model/bp-registration-factory.h',
		'model/bp-registration.h',
		'model/bp-registration-manager.h',
		'model/bp-sdnv.h',
		'model/bp-utility.h',
		'model/bp-rt-epidemic.h',
		'model/bp-rt-prophet.h',
		'model/bp-rt-prophet-hello.h',
		'model/bp-neigh-header.h',
		#'model/bp-hello-header.h',
		'model/bp-rt-trend-of-delivery.h',
		'model/link-life-time.h',
		'model/vector2d.h',
		'model/ieee754.h',
		'model/bp-rt-trend-of-delivery-neigh-hello.h',
		'model/bp-type-tag.h',
		'model/trend-of-delivery.xfs.hpp',
		'model/bp-rt-sprayandwait.h',
		'model/xfuzzy.hpp',
		'helper/bundle-protocol-helper.h',
		#'helper/one-mobility-helper.h',',
		'helper/one-traffic-helper.h',
		'model/ieee754.h'														
        ]

#    if bld.env.ENABLE_EXAMPLES:
#        bld.add_subdirs('examples')

    # bld.ns3_python_bindings()
