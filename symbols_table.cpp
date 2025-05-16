#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

class SymbolTable {
    public:
        // Estructura para almacenar información de los símbolos
        struct SymbolInfo {
            std::string type;
            std::string value;
            bool is_constant = false;
            bool is_function = false;
            std::string return_type;
            std::vector<std::string> parameters;
        };
    
        SymbolTable() {}
    
        // Sección de ámbitos -> begin
    
        // Función para crear un nuevo ámbito vacío y agregarlo a la lista de ámbitos
        void enter_scope() {
            scopes.push_back({});
        }
    
        // Función para salir del ámbito actual, eliminando el último ámbito de la lista
        void exit_scope() {
            if (scopes.size() > 1) {
                scopes.pop_back();
            }
        }
    
        // Sección de ámbitos -> end
    
        // Sección de símbolos -> begin
    
        // Función para agregar un nuevo símbolo al ámbito actual
        void add_symbol(const std::string& name, const std::string& symbol_type, const std::string& value = "", bool is_function = false, bool is_constant = false) {
            // Verifica si el símbolo ya existe en el ámbito actual
            if (scopes.back().count(name) > 0) {
                throw std::runtime_error("Símbolo '" + name + "' ya declarado en este ámbito");
            }
    
            // Agrega el símbolo al ámbito actual
            scopes.back()[name] = {symbol_type, value, is_constant, is_function};
        }
    
        // Método para registrar una función en la tabla de símbolos en el ámbito actual
        void register_function(const std::string& name, const std::string& return_type, const std::vector<std::string>& parameters) {
            // Verifica si la función ya está declarada en el ámbito actual
            if (scopes.back().count(name) > 0) {
                throw std::runtime_error("Función '" + name + "' ya declarada en este ámbito");
            }
    
            // Agrega la función al ámbito actual
            SymbolInfo symbol;
            symbol.is_function = true;
            symbol.return_type = return_type;
            symbol.parameters = parameters;
            scopes.back()[name] = symbol;
        }
    
        // Función para buscar un símbolo en los ámbitos
        // Busca el símbolo en los ámbitos desde el más interno al más externo
        SymbolInfo lookup(const std::string& name) {
            // Busca el símbolo en los ámbitos desde el más interno al más externo
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                if (it->count(name) > 0) {
                    return (*it)[name];
                }
            }
    
            // Si no se encuentra el símbolo, lanza una excepción
            throw std::runtime_error("Símbolo '" + name + "' no declarado");
        }
    
        // Función para actualizar el valor de un símbolo existente
        void update_symbol(const std::string& name, const std::string& value) {
            // Busca el símbolo en los ámbitos desde el más interno al más externo
            for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
                if (it->count(name) > 0) {
                    (*it)[name].value = value;
                    return;
                }
            }
    
            // Si no se encuentra el símbolo, lanza una excepción
            throw std::runtime_error("Símbolo '" + name + "' no declarado");
        }
    
        // Sección de símbolos -> end
    
    private:
        std::vector<std::map<std::string, SymbolInfo>> scopes; // Lista de mapas para manejar los ámbitos
    };