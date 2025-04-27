package lab9;

import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URL;

/**
 * ServerHeaderFetcher.java
 * This program fetches the "Server" header from a given URL using both HTTP and HTTPS protocols.
 * It prints the server information to the console.
 * Usage: java ServerHeaderFetcher <URL1> <URL2> ...
 */

public class ServerHeaderFetcher {

    public static void main(String[] args) {
        if(args.length < 1){
            System.out.println("Usage: java ServerHeaders <URL1> <URL2> ...");
            return;
        }

        for(String address : args){
            System.out.println("Checking URL: " + address);
            HttpFetch(address, Protocol.HTTP);
            HttpFetch(address, Protocol.HTTPS);
            System.out.println("---------------");

        }
    }

    private enum Protocol {
        HTTP("http", 80),
        HTTPS("https", 443);

        private final String name;
        private final int defaultPort;

        Protocol(String name, int defaultPort) {
            this.name = name;
            this.defaultPort = defaultPort;
        }

        public String getName() {
            return name;
        }

        public String getPath() {
            return name + "://";
        }

        public int getDefaultPort() {
            return defaultPort;
        }
    }

    private static void HttpFetch(String address, Protocol protocol) {
        try {
            URI uri = new URI(CreateAbsolutePath(address, protocol));
            URL url = uri.toURL();
            HttpURLConnection connection = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("GET");
            connection.connect();

            String ServerHeader = connection.getHeaderField("Server");
            if (ServerHeader != null) {
                System.out.printf("port %d: %s\n", protocol.getDefaultPort(), ServerHeader);
            } else {
                System.out.println("No Server header found.");
            }
        } catch (Exception e) {
            System.err.println("Error fetching URL: " + e.getMessage());
        }
    }

    private static String CreateAbsolutePath(String address, Protocol protocol) {
        if (!address.startsWith(protocol.getPath())) {
            return protocol.getPath() + address;
        }
        return address;
    }
}
