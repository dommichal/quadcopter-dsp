#include "serial_protocol.h"
#include "microvision_assert.h"

#include <string.h>

static void SerialProtocol_UAVReceiveHandler(SerialProtocol_UAV *serialProtocolUAV, uint8_t *sourceBuffer, size_t sourceBufferSize) {
  ASSERT_OR_RETURN(serialProtocolUAV != NULL, );
  ASSERT_OR_RETURN(sourceBuffer != NULL, );
  ASSERT_OR_RETURN(sourceBufferSize > 0, );

  SerialProtocol *serialProtocol = &serialProtocolUAV->serialProtocol;
  SerialProtocol_PacketType packetType = SerialProtocol_ReadPacketType(sourceBuffer, sourceBufferSize);
  SerialProtocol_Packet packet;
  size_t payloadSize = 0;
  switch (packetType) {
  case SERIAL_PROTOCOL_PACKET_TYPE_CONTROL_OUTPUT:
    payloadSize = SerialProtocol_Deserialize(sourceBuffer, sourceBufferSize, &packet);
    if (payloadSize == sizeof(SerialProtocol_ControlOutputPayload) && serialProtocol->state == SERIAL_PROTOCOL_STATE_CONNECTED) {
      if (serialProtocolUAV->onControlOutput != NULL) {
        serialProtocolUAV->onControlOutput(serialProtocolUAV, (const SerialProtocol_ControlOutputPayload *)packet.payload, serialProtocolUAV->serialProtocol.context);
      }
    }
    break;
  case SERIAL_PROTOCOL_PACKET_TYPE_MICROVISION_TELEMETRY:
    payloadSize = SerialProtocol_Deserialize(sourceBuffer, sourceBufferSize, &packet);
    if (payloadSize == sizeof(SerialProtocol_MicrovisionTelemetryPayload) && serialProtocol->state == SERIAL_PROTOCOL_STATE_CONNECTED) {
      if (serialProtocolUAV->onMicrovisionTelemetry != NULL) {
        serialProtocolUAV->onMicrovisionTelemetry(serialProtocolUAV, (const SerialProtocol_MicrovisionTelemetryPayload *)packet.payload, serialProtocolUAV->serialProtocol.context);
      }
    }
    break;
  default:
    break;
  }
}

static void SerialProtocol_MicrovisionReceiveHandler(SeiralProtocol_Microvision *serialProtocolMicrovision, uint8_t *sourceBuffer,
                                                     size_t sourceBufferSize) {
  ASSERT_OR_RETURN(serialProtocolMicrovision != NULL, );
  ASSERT_OR_RETURN(sourceBuffer != NULL, );
  ASSERT_OR_RETURN(sourceBufferSize > 0, );

  SerialProtocol *serialProtocol = &serialProtocolMicrovision->serialProtocol;
  SerialProtocol_PacketType packetType = SerialProtocol_ReadPacketType(sourceBuffer, sourceBufferSize);
  SerialProtocol_Packet packet;
  size_t payloadSize = 0;
  switch (packetType) {
  case SERIAL_PROTOCOL_PACKET_TYPE_CONNECT:
    payloadSize = SerialProtocol_Deserialize(sourceBuffer, sourceBufferSize, &packet);
    if (payloadSize == sizeof(SerialProtocol_ConnectPayload)) {
      serialProtocol->state = SERIAL_PROTOCOL_STATE_CONNECTED;

      if (serialProtocol->onConnect != NULL) {
        serialProtocol->onConnect(serialProtocol, (SerialProtocol_ConnectPayload *)packet.payload, serialProtocol->context);
      }
    }
    break;
  case SERIAL_PROTOCOL_PACKET_TYPE_DISCONNECT:
    payloadSize = SerialProtocol_Deserialize(sourceBuffer, sourceBufferSize, &packet);
    if (payloadSize == sizeof(SerialProtocol_DisconnectPayload)) {
      serialProtocol->state = SERIAL_PROTOCOL_STATE_DISCONNECTED;

      if (serialProtocol->onDisconnect != NULL) {
        serialProtocol->onDisconnect(serialProtocol, (SerialProtocol_DisconnectPayload *)packet.payload, serialProtocol->context);
      }
    }
    break;
  case SERIAL_PROTOCOL_PACKET_TYPE_CONTROL_INPUT:
    payloadSize = SerialProtocol_Deserialize(sourceBuffer, sourceBufferSize, &packet);
    if (payloadSize == sizeof(SerialProtocol_ControlInputPayload) && serialProtocol->state == SERIAL_PROTOCOL_STATE_CONNECTED) {
      if (serialProtocolMicrovision->onControlInput != NULL) {
        serialProtocolMicrovision->onControlInput(serialProtocolMicrovision, (SerialProtocol_ControlInputPayload *)packet.payload, serialProtocolMicrovision->serialProtocol.context);
      }
    }
    break;
  case SERIAL_PROTOCOL_PACKET_TYPE_UAV_TELEMETRY:
    payloadSize = SerialProtocol_Deserialize(sourceBuffer, sourceBufferSize, &packet);
    if (payloadSize == sizeof(SerialProtocol_UAVTelemetryPayload) && serialProtocol->state == SERIAL_PROTOCOL_STATE_CONNECTED) {
      if (serialProtocolMicrovision->onUAVTelemetry != NULL) {
        serialProtocolMicrovision->onUAVTelemetry(serialProtocolMicrovision, (SerialProtocol_UAVTelemetryPayload *)packet.payload, serialProtocolMicrovision->serialProtocol.context);
      }
    }
    break;
  default:
    break;
  }
}

SerialProtocol_Status SerialProtocol_InitUAV(SerialProtocol_UAV *serialProtocolUAV, SerialProtocol_OnConnect onConnect,
                                             SerialProtocol_OnDisconnect onDisconnect,
                                             SerialProtocol_OnControlOutput onControlOutput,
                                             SerialProtocol_OnMicrovisionTelemetry onMicrovisionTelemetry, void *context) {
  ASSERT_OR_RETURN(serialProtocolUAV != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(onControlOutput != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  memset(serialProtocolUAV, 0, sizeof(*serialProtocolUAV));

  SerialProtocol_RingBuffer_Init(&serialProtocolUAV->serialProtocol.txBuff);
  serialProtocolUAV->serialProtocol.onDisconnect = onDisconnect;
  serialProtocolUAV->serialProtocol.onConnect = onConnect;
  serialProtocolUAV->onMicrovisionTelemetry = onMicrovisionTelemetry;
  serialProtocolUAV->onControlOutput = onControlOutput;
  serialProtocolUAV->serialProtocol.context = context;

  return SERIAL_PROTOCOL_STATUS_OK;
}

SerialProtocol_Status SerialProtocol_InitMicrovision(SeiralProtocol_Microvision *serialProtocolMicrovision, SerialProtocol_OnConnect onConnect,
                                                     SerialProtocol_OnDisconnect onDisconnect,
                                                     SerialProtocol_OnControlInput onControlInput,
                                                     SerialProtocol_OnUAVTelemetry onUAVTelemetry, void *context) {
  ASSERT_OR_RETURN(serialProtocolMicrovision != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(onControlInput != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  memset(serialProtocolMicrovision, 0, sizeof(*serialProtocolMicrovision));
  
  SerialProtocol_RingBuffer_Init(&serialProtocolMicrovision->serialProtocol.txBuff);
  serialProtocolMicrovision->serialProtocol.onDisconnect = onDisconnect;
  serialProtocolMicrovision->serialProtocol.onConnect = onConnect;
  serialProtocolMicrovision->onControlInput = onControlInput;
  serialProtocolMicrovision->onUAVTelemetry = onUAVTelemetry;
  serialProtocolMicrovision->serialProtocol.context = context;

  return SERIAL_PROTOCOL_STATUS_OK;
}

SerialProtocol_Status SerialProtocol_SendPacket(SerialProtocol *serialProtocol, SerialProtocol_Packet *packet, size_t packetSize) {
  ASSERT_OR_RETURN(serialProtocol != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(packet != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(packetSize > 0, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  if (SerialProtocol_RingBuffer_Enqueue(&serialProtocol->txBuff, packet, packetSize)) {
    return SERIAL_PROTOCOL_STATUS_OK;
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}

SerialProtocol_Status SerialProtocol_GetPacketRedyToSend(SerialProtocol *serialProtocol, SerialProtocol_Packet *packetToSend, size_t *packetSize) {
  ASSERT_OR_RETURN(serialProtocol != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(packetToSend != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(packetSize != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  if (!SerialProtocol_RingBuffer_IsEmpty(&serialProtocol->txBuff)) {
    if (SerialProtocol_RingBuffer_Dequeue(&serialProtocol->txBuff, packetToSend, packetSize)) {
      return SERIAL_PROTOCOL_STATUS_OK;
    }
  }
  return SERIAL_PROTOCOL_STATUS_BUFFER_EMPTY;
}

SerialProtocol_Status SerialProtocol_UAVReceivePacket(SerialProtocol_UAV *serialProtocolUAV, uint8_t *sourceBuffer, size_t sourceBufferSize) {
  ASSERT_OR_RETURN(serialProtocolUAV != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(sourceBuffer != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(sourceBufferSize > 0, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_UAVReceiveHandler(serialProtocolUAV, sourceBuffer, sourceBufferSize);
  return SERIAL_PROTOCOL_STATUS_OK;
}

SerialProtocol_Status SerialProtocol_MicrovisionReceivePacket(SeiralProtocol_Microvision *serialProtocolMicrovision, uint8_t *sourceBuffer,
                                                              size_t sourceBufferSize) {
  ASSERT_OR_RETURN(serialProtocolMicrovision != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(sourceBuffer != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(sourceBufferSize > 0, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_MicrovisionReceiveHandler(serialProtocolMicrovision, sourceBuffer, sourceBufferSize);
  return SERIAL_PROTOCOL_STATUS_OK;
}

SerialProtocol_Status SerialProtocol_SendConnect(SerialProtocol *serialProtocol) {
  ASSERT_OR_RETURN(serialProtocol != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_ConnectPayload payload = {
      .status = serialProtocol->connectionInfo.status,
      .connectionId = serialProtocol->connectionInfo.connectionId,
  };
  SerialProtocol_Packet packet;
  size_t packetSize = SerialProtocol_Serialize(SERIAL_PROTOCOL_PACKET_TYPE_CONNECT, &payload, sizeof(payload), &packet);
  if (packetSize > 0 && packetSize <= SERIAL_PROTOCOL_MAX_PACKET_LENGTH) {
    SerialProtocol_Status status = SerialProtocol_SendPacket(serialProtocol, &packet, packetSize);
    if (SERIAL_PROTOCOL_STATUS_OK == status) {
      serialProtocol->state = SERIAL_PROTOCOL_STATE_CONNECTED;
    }
    return status;
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}

SerialProtocol_Status SerialProtocol_SendDisconnect(SerialProtocol *serialProtocol) {
  ASSERT_OR_RETURN(serialProtocol != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_DisconnectPayload payload = {
      .status = serialProtocol->connectionInfo.status,
      .connectionId = serialProtocol->connectionInfo.connectionId,
  };
  SerialProtocol_Packet packet;
  size_t packetSize = SerialProtocol_Serialize(SERIAL_PROTOCOL_PACKET_TYPE_DISCONNECT, &payload, sizeof(payload), &packet);
  if (packetSize > 0 && packetSize <= SERIAL_PROTOCOL_MAX_PACKET_LENGTH) {
    SerialProtocol_Status status = SerialProtocol_SendPacket(serialProtocol, &packet, packetSize);
    if (SERIAL_PROTOCOL_STATUS_OK == status) {
      serialProtocol->state = SERIAL_PROTOCOL_STATE_DISCONNECTED;
      return status;
    }
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}

SerialProtocol_Status SerialProtocol_SendUAVControlInput(SerialProtocol_UAV *serialProtocolUAV, SerialProtocol_ControlInput const *controlInput) {
  ASSERT_OR_RETURN(serialProtocolUAV != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(controlInput != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_ControlInputPayload payload = {
    .velocityX = controlInput->velocityX,
    .velocityY = controlInput->velocityY,
    .velocityZ = controlInput->velocityZ,
    .rollActual = controlInput->rollActual,
    .pitchActual = controlInput->pitchActual,
    .yawActual = controlInput->yawActual,
    .angularVelocityX = controlInput->angularVelocityX,
    .angularVelocityY = controlInput->angularVelocityY,
    .angularVelocityZ = controlInput->angularVelocityZ,
  };
  SerialProtocol_Packet packet;
  size_t packetSize = SerialProtocol_Serialize(SERIAL_PROTOCOL_PACKET_TYPE_CONTROL_INPUT, &payload, sizeof(payload), &packet);
  if (packetSize > 0 && packetSize <= SERIAL_PROTOCOL_MAX_PACKET_LENGTH) {
    return SerialProtocol_SendPacket(&serialProtocolUAV->serialProtocol, &packet, packetSize);
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}

SerialProtocol_Status SerialProtocol_SendMicrovisionControlOutput(SeiralProtocol_Microvision *serialProtocolMicrovision,
                                                                  SerialProtocol_ControlOutput const *controlOutput) {
  ASSERT_OR_RETURN(serialProtocolMicrovision != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(controlOutput != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_ControlOutputPayload payload = {
      .rollTarget = controlOutput->rollTarget,
      .pitchTarget = controlOutput->pitchTarget,
      .yawTarget = controlOutput->yawTarget,
      .thrustTarget = controlOutput->thrustTarget,
  };
  SerialProtocol_Packet packet;
  size_t packetSize = SerialProtocol_Serialize(SERIAL_PROTOCOL_PACKET_TYPE_CONTROL_OUTPUT, &payload, sizeof(payload), &packet);
  if (packetSize > 0 && packetSize <= SERIAL_PROTOCOL_MAX_PACKET_LENGTH) {
    return SerialProtocol_SendPacket(&serialProtocolMicrovision->serialProtocol, &packet, packetSize);
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}

SerialProtocol_Status SerialProtocol_SendMicrovisionTelemetry(SeiralProtocol_Microvision *serialProtocolMicrovision,
                                                              SerialProtocol_MicrovisionTelemetry const *microvisionTelemetry) {
  ASSERT_OR_RETURN(serialProtocolMicrovision != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(microvisionTelemetry != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_MicrovisionTelemetryPayload payload = {
      .elevation = microvisionTelemetry->elevation,
      .velocityX = microvisionTelemetry->velocityX,
      .velocityY = microvisionTelemetry->velocityY,
      .velocityZ = microvisionTelemetry->velocityZ,
      .status = microvisionTelemetry->status.systemStatus,
  };
  SerialProtocol_Packet packet;
  size_t packetSize = SerialProtocol_Serialize(SERIAL_PROTOCOL_PACKET_TYPE_MICROVISION_TELEMETRY, &payload, sizeof(payload), &packet);
  if (packetSize > 0 && packetSize <= SERIAL_PROTOCOL_MAX_PACKET_LENGTH) {
    return SerialProtocol_SendPacket(&serialProtocolMicrovision->serialProtocol, &packet, packetSize);
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}

SerialProtocol_Status SerialProtocol_SendUAVTelemetry(SerialProtocol_UAV *serialProtocolUAV, SerialProtocol_UAVTelemetry const *uavTelemetry) {
  ASSERT_OR_RETURN(serialProtocolUAV != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);
  ASSERT_OR_RETURN(uavTelemetry != NULL, SERIAL_PROTOCOL_STATUS_INVALID_INPUT_ARGUMENT);

  SerialProtocol_UAVTelemetryPayload payload = {
      .roll = uavTelemetry->roll,
      .pitch = uavTelemetry->pitch,
      .yaw = uavTelemetry->yaw,
      .angularVelocityX = uavTelemetry->angularVelocityX,
      .angularVelocityY = uavTelemetry->angularVelocityY,
      .angularVelocityZ = uavTelemetry->angularVelocityZ,
      .status = uavTelemetry->status.systemStatus,
  };
  SerialProtocol_Packet packet;
  size_t packetSize = SerialProtocol_Serialize(SERIAL_PROTOCOL_PACKET_TYPE_UAV_TELEMETRY, &payload, sizeof(payload), &packet);
  if (packetSize > 0 && packetSize <= SERIAL_PROTOCOL_MAX_PACKET_LENGTH) {
    return SerialProtocol_SendPacket(&serialProtocolUAV->serialProtocol, &packet, packetSize);
  }
  return SERIAL_PROTOCOL_STATUS_INVALID_PACKET;
}
