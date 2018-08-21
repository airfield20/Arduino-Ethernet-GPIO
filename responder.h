//
// Created by aaron on 20/08/18.
//

#ifndef FISH_ROOM_FIRMWARE_TESTS_RESPONDER_H
#define FISH_ROOM_FIRMWARE_TESTS_RESPONDER_H

#define GET 22
#define POST 33
#define PUT 44
#define DELETE 55
#define UNRECOGNIZED -1
#define MAX_NUM_VARS 16
#define MAX_NUM_PINS 16

namespace mt {
  template <typename T, typename S>
    struct pair{
      T first;
      S second;
      pair(const T& f, const S& s)
      {
        first = f;
        second = s;
      }
      pair(){}  
    };

    struct request {
        unsigned int type;
        String path;
        IPAddress host;
        String referer;
        pair<String, String> vars[MAX_NUM_VARS];
        unsigned int num_vars;
        request(const unsigned int &itype, const String &ipath, const IPAddress &ihost, const String &ireferer) {
            type = itype;
            path = ipath;
            host = ihost;
            referer = ireferer;
        }

        request() {}
    };

    

    String serialize_request(const request& req)
    {
//        Serial.println("Starting serialization");
        String str = "TYPE: ";
        switch(req.type)
        {
            case 22:
                str.concat("GET");
                break;
            case 33:
                str.concat("POST");
                break;
            case 44:
                str.concat("PUT");
                break;
            case 55:
                str.concat("DELETE");
                break;
            default:
                str.concat("UNRECOGNIZED");
        }
        str.concat(", PATH: ");
        str.concat(req.path);
        str.concat(", HOST: ");
        str.concat(req.host[0]);
        str.concat(".");
        str.concat(req.host[1]);
        str.concat(".");
        str.concat(req.host[2]);
        str.concat(".");
        str.concat(req.host[3]);
        str.concat(", REFERER: ");
        str.concat(req.referer);
        str.concat(", VARS: ");
        for(unsigned int i = 0; i < req.num_vars; ++i)
        {
          str.concat(req.vars[i].first);
          str.concat(" : ");
          str.concat(req.vars[i].second);
          str.concat(", ");
        }
        
//        Serial.println("Done serializing");
        return str;
    }

int parse_vars(const String& str, pair<String,String>* kv, const unsigned int& isize)
{
    //pair<String,String>* vars = new pair<String,String>[16];
    unsigned int num_vars = 0, idx = 0;
    while(idx < str.length() && num_vars < isize)
    {
        String key = "", value = "";
        while(str[idx] != '=' && idx < str.length())
        {
            key.concat(str[idx]);
            ++idx;
        }
        if(str[idx] != '=')
        {
            break;
        }
        ++idx;
        while(str[idx] != '&' && idx < str.length())
        {
            value.concat(str[idx]);
            ++idx;
        }
        kv[num_vars].first = key;
        kv[num_vars].second = value;
        ++idx;
        key = "";
        value = "";
        ++num_vars;
    }
    return num_vars;
}

/*
    GET /aaronisdope?potato=planted HTTP/1.1
    Host: 10.42.0.177
    Connection: keep-alive
    Cache-Control: max-age=0
    Upgrade-Insecure-Requests: 1
    User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36
    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,;q=0.8
    Referer: http://10.42.0.177/aaronisdope?potato=planted
    Accept-Encoding: gzip, deflate
    Accept-Language: en-US,en;q=0.9

    client disconnected
     */

    request parse_request(const String& str)
    {
//        Serial.println("Starting parsing");
        request req(22,"/dope",IPAddress(123,32,12,4),"not_parsed");
//        Serial.println("SP0");
        bool t = false, p = false, addr = false;
        for(unsigned int i = 0; i < str.length(); ++i)
        {
//            Serial.println("SP1");
            if(str.charAt(i) == '\n' || str.charAt(i) == '\r'  )
            {
                continue;
            }
            else
            {
//              Serial.println("SP2");
              if(!t)
              {
//                Serial.println("SP3");
                String type = "";
                while(str.charAt(i) != ' ')
                {
                    type.concat(str.charAt(i));
                    ++i;
                }
                ++i;
                if(type == "GET")
                {
                    req.type = GET;
                }
                else if(type == "POST")
                {
                    req.type = POST;
                }
                else if(type == "PUT")
                {
                    req.type = PUT;
                }
                else if(type == "DELETE")
                {
                    req.type = DELETE;
                }
                else{
                    req.type = UNRECOGNIZED;
                }
//                Serial.println("type is " + String(type));
                t = true;
                
              }
              if(t && !p)
              {
//                Serial.println("SP4");
                String path = "";
                while(str.charAt(i) != '?')
                {
                    path.concat(str.charAt(i));
                    ++i;
                }
                ++i;
                String var_list = "";
                while(str.charAt(i) != ' ')
                {
                    var_list.concat(str.charAt(i));
                    ++i;
                }
                req.num_vars = parse_vars(var_list, req.vars, MAX_NUM_VARS);
                ++i;
                p = true;
                req.path = path;
              }
              if(t && p && !addr)
              {
//                Serial.println("SP5");
                String ip = "";
                while(str.charAt(i) != '\n')
                {
                    ++i;
                }
                while(str.charAt(i) != ' ')
                {
                    ++i;
                }
                ++i;
                while(str.charAt(i) != '\n')
                {
                    ip.concat(str.charAt(i));
                    ++i;
                }
                req.host.fromString(ip);
                addr = true;
                ++i;
                break;
              }
            }
        }
//        Serial.println("Done parsing");
        return req;
    }

void process_req(const pair<String,String> vars[], const int& num_vars)
{
  if(num_vars <= 2 && num_vars > 0){
    if(vars[0].first == "pin")
    {
        int pin = vars[0].second.toInt();
        int state = LOW;
//        Serial.println(vars[1].first);
        if(vars[1].first == "state")
        {
            state = vars[1].second.toInt();
            digitalWrite(pin, state);
            return;
        }
        else {
          Serial.println("malformed request");
          return;
        }
    }
    else if(vars[0].first == "pins")
    {
        int pins[MAX_NUM_PINS];
        String num = "";
        int idx = 0;
        for(unsigned int i = 0; i < vars[0].second.length(); ++i)
        {
            if(vars[0].second[i] == ',')
            {
                pins[idx] = num.toInt();
                num = "";
                ++idx;
            }
            if(vars[0].second[i] != ',') {
                num.concat(vars[0].second[i]);
            }
        }
        pins[idx] = num.toInt();
        num = "";
        ++idx;
        int states[MAX_NUM_PINS];
        int idx2 = 0;
        if(vars[1].first == "states") {
            for(unsigned int i = 0; i < vars[1].second.length(); ++i) {
                    if(vars[1].second[i] != ',' && (vars[1].second[i] == '1' || vars[1].second[i] == '0') )
                    {
                        states[idx2] = vars[1].second[i] - 48;
                        ++idx2;
                    }
            }

            if(idx == idx2)
            {
                for(int i = 0; i < idx; ++i)
                {
                    digitalWrite(pins[i], states[i]);
                }
            }
            else
            {
                Serial.println("malformed request");
            }
  
        }
        else {
          Serial.println("malformed request");
          return;
        }
        return;
    }
    else if(vars[0].first == "all")
    {
        if(vars[0].second == "HIGH")
        {
            for(int i = 0; i < MAX_NUM_PINS; ++i)
            {
                digitalWrite(i, HIGH);
            }
        }
        else if(vars[0].second == "LOW")
        {
            for(int i = 0; i < MAX_NUM_PINS; ++i)
            {
                digitalWrite(i, LOW);
            }
        }
        else{
          Serial.println("malformed request");
          return;
        }
        return;
    }
  }
    Serial.println("malformed request");
}

    void send_response(EthernetClient &client) {
        // send a standard http response header
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");  // the connection will be closed after completion of the response
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html>");
        // output the value of each analog input pin
        for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
        }
        client.println("</html>");
    }
}
#endif //FISH_ROOM_FIRMWARE_TESTS_RESPONDER_H

