#ifndef GRAPHS_MAPS_PRINTER_H_
#define GRAPHS_MAPS_PRINTER_H_
void pretty_print_cost_map(char *buffer,
			   struct CostMap *map,
			   struct Endpoints *pts);

void cost_map_to_csv(char *filename,
		     struct CostMap *map);
#endif /* ifndef GRAPHS_MAPS_PRINTER_H_ */
