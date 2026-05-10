import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

/**
 * LOG FILE PARSER — Calsoft Assignment 3, Q1
 * ---------------------------------------------------------------
 * HOW TO RUN IN VS CODE TERMINAL:
 *
 *   Compile:
 *     javac LogParser.java
 *
 *   Run:
 *     java LogParser "Log_19_10_17_11_42_01.log"
 *     java LogParser "Log_19_10_17_11_42_01.log" 5
 *     java LogParser "Log_19_10_17_11_42_01.log" 5 error
 *     java LogParser "Log_19_10_17_11_42_01.log" 10 error,info
 *     java LogParser "Log_19_10_17_11_42_01.log" 10 error,info,debug,warning
 *
 * PARAMETERS:
 *   1. filePath  — path to log file         (REQUIRED)
 *   2. numLines  — how many lines to show   (OPTIONAL, default = 10)
 *   3. logTypes  — comma separated types    (OPTIONAL, default = error)
 *                  Allowed: error, warning, info, debug
 * ---------------------------------------------------------------
 */
public class LogParser {

    // ── Allowed log types ────────────────────────────────────
    private static final Set<String> VALID_TYPES = new HashSet<>(
        Arrays.asList("error", "warning", "info", "debug")
    );

    // ── Custom Exceptions ────────────────────────────────────

    static class InvalidFilePathException extends Exception {
        InvalidFilePathException(String path) {
            super("ERROR: File not found -> \"" + path + "\"");
        }
    }

    static class InvalidLogTypeException extends Exception {
        InvalidLogTypeException(String type) {
            super("ERROR: Invalid log type -> \"" + type
                + "\". Allowed values: error, warning, info, debug");
        }
    }

    // ── MAIN — entry point ───────────────────────────────────

    public static void main(String[] args) {

        // No arguments given
        if (args.length < 1) {
            System.out.println("Usage:");
            System.out.println("  java LogParser <filePath> [numLines] [logTypes]");
            System.out.println();
            System.out.println("Examples:");
            System.out.println("  java LogParser \"Log_19_10_17_11_42_01.log\"");
            System.out.println("  java LogParser \"Log_19_10_17_11_42_01.log\" 5");
            System.out.println("  java LogParser \"Log_19_10_17_11_42_01.log\" 10 error");
            System.out.println("  java LogParser \"Log_19_10_17_11_42_01.log\" 10 error,warning");
            return;
        }

        // Argument 1 — file path (required)
        String filePath = args[0];

        // Argument 2 — numLines (optional, default 10)
        int numLines = 10;
        if (args.length >= 2) {
            try {
                numLines = Integer.parseInt(args[1]);
                if (numLines <= 0) {
                    System.out.println("ERROR: numLines must be a positive number.");
                    return;
                }
            } catch (NumberFormatException e) {
                System.out.println("ERROR: numLines must be a number. Got: \"" + args[1] + "\"");
                return;
            }
        }

        // Argument 3 — logTypes (optional, default "error")
        String logTypes = "error";
        if (args.length >= 3) {
            logTypes = args[2];
        }

        // Run parser and show output
        LogParser parser = new LogParser();
        try {
            List<String> results = parser.getFilteredLogs(filePath, numLines, logTypes);

            if (results.isEmpty()) {
                System.out.println("No logs found for type(s): [" + logTypes + "]");
            } else {
                System.out.println("============================================================");
                System.out.println("  File    : " + filePath);
                System.out.println("  Type(s) : " + logTypes);
                System.out.println("  Showing : " + results.size() + " most recent line(s)");
                System.out.println("============================================================");
                for (String line : results) {
                    System.out.println(line);
                }
                System.out.println("============================================================");
            }

        } catch (InvalidFilePathException | InvalidLogTypeException e) {
            System.out.println(e.getMessage());
        }
    }

    // ── Core Method ──────────────────────────────────────────

    /**
     * Reads log file from END (most recent first) and
     * returns N lines matching the given log type(s).
     */
    public List<String> getFilteredLogs(String filePath,
                                         int numLines,
                                         String logTypes)
            throws InvalidFilePathException, InvalidLogTypeException {

        // Validate file path
        File file = new File(filePath);
        if (!file.exists() || !file.isFile()) {
            throw new InvalidFilePathException(filePath);
        }

        // Validate and parse log types
        Set<String> typesSet = validateAndParseTypes(logTypes);

        // Read all lines from the file
        List<String> allLines = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                allLines.add(line.trim());
            }
        } catch (IOException e) {
            throw new InvalidFilePathException(filePath);
        }

        // Scan from BOTTOM → collect most recent matching lines
        LinkedList<String> result = new LinkedList<>();
        for (int i = allLines.size() - 1; i >= 0; i--) {
            if (result.size() == numLines) break;
            String line = allLines.get(i);
            if (isMatchingLog(line, typesSet)) {
                result.addFirst(line); // prepend to keep chronological order
            }
        }

        return result;
    }

    // ── Helper: validate types string ────────────────────────

    private Set<String> validateAndParseTypes(String logTypes)
            throws InvalidLogTypeException {

        Set<String> result = new HashSet<>();
        for (String part : logTypes.split(",")) {
            String type = part.trim().toLowerCase();
            if (!VALID_TYPES.contains(type)) {
                throw new InvalidLogTypeException(type);
            }
            result.add(type);
        }
        return result;
    }

    // ── Helper: check if line matches requested type ─────────

    private boolean isMatchingLog(String line, Set<String> types) {
        if (line == null || line.isEmpty()) return false;
        if (!line.startsWith("["))          return false;
        int closeBracket = line.indexOf(']');
        if (closeBracket <= 1)              return false;
        String tag = line.substring(1, closeBracket).toLowerCase().trim();
        return types.contains(tag);
    }
}
