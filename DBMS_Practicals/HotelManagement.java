import java.sql.*;
import java.util.Scanner;

public class HotelManagement {

    private static final String URL = "jdbc:mysql://10.10.14.233/te31470_db";
    private static final String USER = "te31470";
    private static final String PASSWORD = "te31470"; 

    // Method to establish the database connection
    public static Connection getConnection() throws SQLException {
        return DriverManager.getConnection(URL, USER, PASSWORD);
    }

    // Method to insert a new guest
    public static void addGuest(Scanner sc) {
        System.out.print("Enter guest name: ");
        String name = sc.nextLine();
        System.out.print("Enter guest email: ");
        String email = sc.nextLine();
        System.out.print("Enter guest phone: ");
        String phone = sc.nextLine();
        System.out.print("Enter guest address: ");
        String address = sc.nextLine();

        try (Connection conn = getConnection()) {
            String query = "INSERT INTO guests (name, email, phone, address) VALUES (?, ?, ?, ?)";
            try (PreparedStatement stmt = conn.prepareStatement(query)) {
                stmt.setString(1, name);
                stmt.setString(2, email);
                stmt.setString(3, phone);
                stmt.setString(4, address);
                stmt.executeUpdate();
                System.out.println("Guest added successfully!");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    // Method to update guest information
    public static void updateGuest(Scanner sc) {
        System.out.print("Enter guest ID to update: ");
        int guestId = sc.nextInt();
        sc.nextLine(); // Consume the newline
        System.out.print("Enter new guest name: ");
        String name = sc.nextLine();
        System.out.print("Enter new guest email: ");
        String email = sc.nextLine();
        System.out.print("Enter new guest phone: ");
        String phone = sc.nextLine();
        System.out.print("Enter new guest address: ");
        String address = sc.nextLine();

        try (Connection conn = getConnection()) {
            String query = "UPDATE guests SET name = ?, email = ?, phone = ?, address = ? WHERE guest_id = ?";
            try (PreparedStatement stmt = conn.prepareStatement(query)) {
                stmt.setString(1, name);
                stmt.setString(2, email);
                stmt.setString(3, phone);
                stmt.setString(4, address);
                stmt.setInt(5, guestId);
                stmt.executeUpdate();
                System.out.println("Guest updated successfully!");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    // Method to delete a guest
    public static void deleteGuest(Scanner sc) {
        System.out.print("Enter guest ID to delete: ");
        int guestId = sc.nextInt();

        try (Connection conn = getConnection()) {
            String query = "DELETE FROM guests WHERE guest_id = ?";
            try (PreparedStatement stmt = conn.prepareStatement(query)) {
                stmt.setInt(1, guestId);
                stmt.executeUpdate();
                System.out.println("Guest deleted successfully!");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    // Method to view all guests
    public static void viewGuests() {
        try (Connection conn = getConnection()) {
            String query = "SELECT * FROM guests";
            try (Statement stmt = conn.createStatement(); ResultSet rs = stmt.executeQuery(query)) {
                while (rs.next()) {
                    System.out.println("ID: " + rs.getInt("guest_id") + ", Name: " + rs.getString("name"));
                }
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }

    // Main method for the CLI interface
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int choice;

        while (true) {
            System.out.println("Hotel Management System");
            System.out.println("1. Add Guest");
            System.out.println("2. Update Guest");
            System.out.println("3. Delete Guest");
            System.out.println("4. View Guests");
            System.out.println("5. Exit");
            System.out.print("Enter your choice: ");
            choice = sc.nextInt();
            sc.nextLine(); // Consume the newline

            switch (choice) {
                case 1:
                    addGuest(sc);
                    break;
                case 2:
                    updateGuest(sc);
                    break;
                case 3:
                    deleteGuest(sc);
                    break;
                case 4:
                    viewGuests();
                    break;
                case 5:
                    System.out.println("Exiting...");
                    return;
                default:
                    System.out.println("Invalid choice. Please try again.");
            }
        }
    }
}
