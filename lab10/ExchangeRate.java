package lab10;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;

import java.io.IOException;


public class ExchangeRate {
    private static final String address = "https://www.x-rates.com/calculator/?from=%s&to=%s&amount=1";

    public static void main(String[] args) throws IOException {

        if (args.length < 2) {
            System.out.println("Usage: java ExchangeRate <from_currency> <to_currency> in ISO 4217 format");
            return;
        }
        String fromCurrency = args[0].toUpperCase();
        String toCurrency = args[1].toUpperCase();

        String url = String.format(address, fromCurrency, toCurrency);

        Document doc = Jsoup.connect(url)
                .userAgent("Mozilla/5.0")
                .timeout(5000)
                .get();

        String rate = doc.select("span[class='ccOutputRslt']").first().ownText().strip();

        if(rate.equals("0.00")) {
            System.out.println("Error: wrong currency code");
            return;
        }

        System.out.printf("Exchange rate %s to %s:\n", fromCurrency, toCurrency);
        System.out.println(rate);

    }

}
