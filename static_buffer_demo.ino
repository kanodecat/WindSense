/**
This program receives raw image data, uses a local comuter vision model to determine the presence of people in the image, then publishes the value to an MQTT server
**/
#include <Person_detection_INRIA_inferencing.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char *ssid = "";
const char *password = "";

// MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io";  // EMQX broker endpoint
const char *mqtt_topic = "";     // MQTT topic
const char *mqtt_username = "";  // MQTT username for authentication
const char *mqtt_password = "";  // MQTT password for authentication
const int mqtt_port = 1883;  // MQTT port (TCP)

static const float features[] = {
  
    // copy raw features here (for example from the 'Live classification' page)
    // see https://docs.edgeimpulse.com/docs/running-your-impulse-arduino
    0x9e9892, 0xa19b96, 0xa29d97, 0xa39e98, 0xa09b96, 0xa4a09a, 0xa7a49e, 0xaba8a2, 0xaeaaa5, 0xa9a49f, 0xa8a39d, 0xaba7a1, 0xa9a59f, 0xa8a49e, 0xa5a09b, 0xa39f9a, 0xa29e99, 0xa09b96, 0x9f9b95, 0x9a9590, 0xc4c2bf, 0xc5c3c1, 0xc9c8c4, 0xcac9c6, 0xc1c0bd, 0xc8c7c3, 0xcecfcc, 0xc0bdb8, 0xbfbcb7, 0xcecfcc, 0xc7c7c4, 0xd1d1cd, 0xcececb, 0xcfcfcc, 0xcececa, 0xcbcbc8, 0xcacac7, 0xcac9c6, 0xcfcfcc, 0xcacac7, 0xb7b4b0, 0xb9b6b2, 0xbcb9b5, 0xbcb9b6, 0xb5b3af, 0xbab7b3, 0xc1c0bc, 0x9b8b86, 0x92817d, 0xbebdb9, 0xbab9b5, 0xc8c7c3, 0xc8c7c3, 0xc6c6c3, 0xc5c4c1, 0xc6c5c2, 0xc6c5c2, 0xc2c2be, 0xc8c8c4, 0xc7c7c3, 0xa7a29e, 0xa5a29d, 0xa5a19c, 0xaaa7a3, 0xa8a6a1, 0xaaa7a2, 0xb0afaa, 0xb4a3a1, 0x9b8887, 0xaeaea9, 0xa09c97, 0x8e8b82, 0x908e86, 0x949188, 0x928e86, 0x928f88, 0x9d9992, 0xacaaa5, 0xa7a5a1, 0xa6a49f, 0xbfbcb9, 0xbcbab8, 0xbab7b5, 0xc3c1be, 0xc1c0be, 0xc9c8c5, 0xaeb0ad, 0x585754, 0x322e2c, 0x6b6d6c, 0xa1a09b, 0x636051, 0x626051, 0x7b7668, 0x6d685a, 0x5d5c4d, 0x8d897a, 0xc0bdb8, 0xb8b7b6, 0xb7b7b4, 0xadaaa4, 0xaaa7a4, 0xa7a3a0, 0xaeaca8, 0xb0afab, 0xb8b7b3, 0x484a48, 0x201, 0xa0b0b, 0x60707, 0x71716e, 0x79786e, 0x707068, 0x96958d, 0x898781, 0x7b7b70, 0xadaba0, 0xc3c3be, 0xbec0bf, 0xb9bab9, 0xa3a09b, 0x999692, 0x9c9a96, 0x9f9d99, 0x9f9d99, 0x979590, 0x272827, 0xa0b0b, 0x121212, 0x30404, 0x3e3e3b, 0x515146, 0x3c3d33, 0x84837a, 0x636259, 0x404132, 0x939285, 0xb3b1ae, 0x9c9b98, 0x9b9a97, 0xafaca9, 0xaaa7a6, 0xb0aeac, 0xafadab, 0xafaeac, 0xa09f9d, 0x181919, 0x90a0a, 0xd0e0d, 0xe0e0e, 0x222220, 0x626054, 0x646251, 0x949282, 0x6a6652, 0x4e4d3d, 0x9a9a90, 0xc2c1be, 0xacaba9, 0xafafad, 0x9e9b97, 0x92908c, 0x9a9893, 0x9d9b97, 0x9e9c99, 0x878785, 0x121212, 0xb0b0b, 0xa0a0a, 0x1a1a1a, 0x262624, 0x84837d, 0x9b9992, 0x9c9a92, 0x9c9a91, 0x9a978e, 0xaaa89e, 0xb9b8b5, 0xaaaaa8, 0xb1b0ae, 0x9e9d99, 0x9f9e9a, 0xa09f9c, 0xa09f9c, 0xa4a39f, 0x8f8f8a, 0x121212, 0xb0c0c, 0xc0d0d, 0x90a0b, 0x484949, 0xa6a6a2, 0x9d9d9b, 0xa7a7a3, 0xaaaaa6, 0xa8a8a4, 0xabaaa5, 0xa6a4a1, 0x9d9c99, 0x9c9b97, 0x9c9a93, 0xa8a6a1, 0xaaa79d, 0xa7a49a, 0x9a9991, 0x94948f, 0x1a1c1c, 0xa0a0a, 0xb0b0a, 0xf1112, 0x787978, 0x979795, 0x949492, 0x9f9e99, 0x9e9e99, 0x9d9d99, 0x9b9b99, 0xa1a19e, 0xa1a09d, 0x9d9d9a, 0x9f9b93, 0xaaa89f, 0xa9a69c, 0xa7a499, 0x9f9e98, 0xa1a29e, 0x484a4a, 0x90a09, 0x50606, 0x3f3f3d, 0x90908b, 0x737471, 0x9a9b95, 0xa5a59d, 0xa3a39c, 0xa2a19b, 0xa2a29e, 0xa0a19f, 0xa0a2a1, 0x9c9e9c, 0x888580, 0x87847f, 0x898884, 0x888783, 0x848380, 0x898984, 0x686966, 0x171717, 0x171717, 0x545350, 0x999893, 0x656563, 0x8c8b87, 0x93938d, 0x91908a, 0x91918b, 0x969692, 0x999a97, 0x979897, 0x929493, 0x686864, 0x656560, 0x656460, 0x676661, 0x6b6a66, 0x72716d, 0x5c5c59, 0x1c1d1d, 0x232524, 0x373837, 0x767672, 0x3e413f, 0x464847, 0x7a7a75, 0x81817c, 0x807f7b, 0x838380, 0x848481, 0x858480, 0x82817d, 0x969ba3, 0x9499a0, 0x888d92, 0x898d92, 0x777b7d, 0x727576, 0x606363, 0x242626, 0x353533, 0x262725, 0x4a4c4a, 0x383a38, 0x464542, 0x4d4f4c, 0x70706c, 0x7c7c78, 0x6b6a66, 0x656562, 0x656562, 0x676764, 0xcfd3e2, 0xdde3f2, 0xc7ccda, 0xdde3f2, 0xcfd7e4, 0xccd3e0, 0x979da6, 0x3c3e41, 0x585859, 0x2c2d2d, 0x7c8289, 0x65676b, 0x6b6562, 0x575c5f, 0x5a5d5e, 0x6e7173, 0x626566, 0x676a6c, 0x626565, 0x616565, 0xced1df, 0xd4d8e6, 0xcfd3e0, 0xd4d8e5, 0xd2d7e4, 0xbabfca, 0xa1a7b0, 0x4d5155, 0x44484c, 0x515559, 0xacb0bb, 0x7c8089, 0x7d848e, 0x777c83, 0x65696e, 0x73777c, 0x858a92, 0x898f97, 0x82868d, 0x7e8289, 0xcbcfdb, 0xcdd0dd, 0xd2d5e3, 0xd2d6e3, 0xcdd2de, 0xccd1dc, 0xb3b8c2, 0x5a5d62, 0x6e7176, 0x65686d, 0xc1c4cf, 0x868a92, 0x9fa3ad, 0xa2a5b0, 0x888b95, 0xa1a7b1, 0xb4b8c6, 0xbdc2d2, 0xacb1bf, 0x9ba0ab, 0xacaeb8, 0xc4c6d3, 0xcccfdb, 0xccd0dd, 0xcacedb, 0xd5dae8, 0x8a9099, 0x646a73, 0x868b94, 0x585c62, 0xb9bcc8, 0x969ba3, 0x9ea1ab, 0xb2aaad, 0x8a8c94, 0xcbcfde, 0xc4c6d5, 0xb8bcca, 0xbfc3d3, 0xb3b8c5, 0xb3b2b8, 0xbdbec4, 0xbebec6, 0xbec0c8, 0xbcbdc6, 0xbdbec7, 0xb5b8c1, 0xbbbfc9, 0xb8bdc8, 0xa2a8b2, 0xbec2ce, 0xb6bac4, 0x7d8188, 0x818189, 0x979ba3, 0xcdcfdb, 0xc2c4d0, 0xc5c7d3, 0xc8cad8, 0xcbcddc
};

WiFiClient espClient;
PubSubClient mqtt_client(espClient);


int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

void connectToWiFi();

void connectToMQTTBroker();

void print_inference_result(ei_impulse_result_t result);


void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    connectToWiFi();
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    connectToMQTTBroker(); 
    
    while (!Serial);
    Serial.println("Edge Impulse Inferencing Demo");
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the WiFi network");
}


void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP8266 ^^");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}


void loop()
{
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        delay(1000);
        return;
    }

    ei_impulse_result_t result = { 0 };

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    if (res != EI_IMPULSE_OK) {
        ei_printf("ERR: Failed to run classifier (%d)\n", res);
        return;
    }

    // print inference return code
    ei_printf("run_classifier returned: %d\r\n", res);
    print_inference_result(result);

    delay(1000);
}

void print_inference_result(ei_impulse_result_t result) {

    // Print how long it took to perform inference
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
            result.timing.dsp,
            result.timing.classification,
            result.timing.anomaly);

    // Print the prediction results (object detection)
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_printf("Object detection bounding boxes:\r\n");
    for (uint32_t i = 0; i < result.bounding_boxes_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }

    // Print the prediction results (classification)
#else
    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);

        if (!mqtt_client.connected())
        {
          connectToMQTTBroker;
        }
        //Publishing to MQTT based on results
        mqtt_client.loop();
        if(mqtt_client.connected())
        {
          if(result.classification[1].value>result.classification[0].value)
          {
            mqtt_client.publish(mqtt_topic, "PERSON DETECTED");
          }
          else
          {
            mqtt_client.publish(mqtt_topic, "BUS STOP EMPTY");
          }
        }
    }
#endif

    // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY
    ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif

#if EI_CLASSIFIER_HAS_VISUAL_ANOMALY
    ei_printf("Visual anomalies:\r\n");
    for (uint32_t i = 0; i < result.visual_ad_count; i++) {
        ei_impulse_result_bounding_box_t bb = result.visual_ad_grid_cells[i];
        if (bb.value == 0) {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                bb.label,
                bb.value,
                bb.x,
                bb.y,
                bb.width,
                bb.height);
    }
#endif

}