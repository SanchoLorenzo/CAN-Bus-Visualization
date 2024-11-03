#include <Arduino_CAN.h>

static uint32_t const CAN_ID = 0x30;  // ID diferente para este dispositivo
static uint32_t msg_cnt = 0;  // Contador de mensajes

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("Starting CAN...");

  // Inicializa el bus CAN a 125 kbps
  if (!CAN.begin(CanBitRate::BR_125k)) {
    Serial.println("CAN.begin(...) failed. Check wiring or transceiver.");
    while (1);
  }

  // Opcional: habilitar el modo loopback interno para pruebas
  CAN.enableInternalLoopback();  // Habilitar loopback para prueba interna (quitar esto si no es necesario)

  Serial.println("CAN initialization successful.");
}

void loop() {
  int errorCode;
  if (CAN.isError(errorCode)) {
    Serial.print("CAN error detected with error code: ");
    Serial.println(errorCode);
    CAN.clearError();  // Restablecer el error
    delay(1000);
    return;
  }

  // Enviar un mensaje cada 5 ciclos del loop
  if (msg_cnt % 5 == 0) {
    uint8_t msg_data[] = { 0x0F, 0xBB, 0, 0, 0, 0, 0, 0 }; // Cambiar datos si es necesario
    memcpy((void *)(msg_data + 4), &msg_cnt, sizeof(msg_cnt)); // Copiar el contador

    // Usamos CanStandardId para definir el ID del mensaje
    CanMsg msg(CanStandardId(CAN_ID), sizeof(msg_data), msg_data);

    int result = CAN.write(msg);
    if (result < 0) {
      Serial.print("CAN.write(...) failed with error code: ");
      Serial.println(result);
    } else {
      Serial.println("Message sent successfully.");
    }
  }

  msg_cnt++;

  // Recibir y leer mensajes CAN
  if (CAN.available()) {
    CanMsg msg = CAN.read();  // Leer mensaje CAN recibido

    // Verificar si el mensaje es estándar o extendido y mostrar su ID
    if (msg.isStandardId()) {
      Serial.print("Received standard message with ID: 0x");
      Serial.println(msg.getStandardId(), HEX);
    } else if (msg.isExtendedId()) {
      Serial.print("Received extended message with ID: 0x");
      Serial.println(msg.getExtendedId(), HEX);
    }

    // Mostrar los datos recibidos (asumimos 8 bytes máximo)
    Serial.print("Data: ");
    for (int i = 0; i < 8; i++) {  // Asumimos que el mensaje tiene hasta 8 bytes de datos
      Serial.print(msg.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  delay(1000);  // Delay para evitar saturación del bus
}

