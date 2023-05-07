#!/ usr / bin / env python3

import socket import Blaster.Messages.MessageType as messageTypes import Blaster.Messages.BlasterMessage as msg import Blaster.Messages.VisualObject as vObj

    IP = "127.0.0.1" UDP_PORT = 2000

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

                                          s.bind((IP, UDP_PORT))

                                                      while True : data = s.recv(1024) try :
#Deserializing root message
                                                  message
= msg.BlasterMessage
      .GetRootAsBlasterMessage(data, 0)

#Detecting message object type and initializing that type
#(in this example we only have "VisualObject" - type
          if message.MessageType() == messageTypes.MessageType.VisualObject
    : visualObject =
      vObj.VisualObject()
          visualObject.Init(message.Message().Bytes, message.Message().Pos)

#Print horizontal speed for demo
              print(visualObject.Vx())

                  except Exception as e : print(e)
