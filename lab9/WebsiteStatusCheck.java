package lab9;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;

public class WebsiteStatusCheck {
    private static final String address = "http://th.if.uj.edu.pl/";
    private static final String text = "Institute of Theoretical Physics";

    public static void main(String[] args) throws URISyntaxException {
        try{
            URI uri = new URI(address);
            URL url = uri.toURL();

            HttpURLConnection connection    = (HttpURLConnection) url.openConnection();
            connection.setRequestMethod("GET");
            connection.setConnectTimeout(5000);
            connection.setReadTimeout(5000);
            connection.setRequestProperty("User-Agent", "Mozilla/5.0");
            connection.connect();

            int statusCode = connection.getResponseCode();
            String contentType = connection.getContentType();

            if (statusCode != 200) {
                System.err.println("Error: wrong http status: " + statusCode);
                System.exit(1);
            }

            if (contentType == null || !contentType.startsWith("text/html")) {
                System.err.println("Error: wrong content type Content-Type: " + contentType);
                System.exit(1);
            }

            String content = readWebSiteContent(connection.getInputStream());

            if(!content.contains(text)) {
                System.err.println("Error: text not found");
                System.exit(1);
            }

            System.out.println("Success");
            System.exit(0);


        }catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
            System.exit(1);
        }

    }

    private static String readWebSiteContent(InputStream inputStream) {
        BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
        StringBuilder content = new StringBuilder();
        String line;

        try {
            while ((line = reader.readLine()) != null) {
                content.append(line).append("\n");
            }
            reader.close();
        } catch (Exception e) {
            System.err.println("Error reading content: " + e.getMessage());
        }

        return content.toString();
    }
}
